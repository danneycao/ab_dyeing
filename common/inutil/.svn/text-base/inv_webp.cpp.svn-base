#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "webp/encode.h"

#include <png.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <string>
#include <iostream>
#include <fstream>

#include "util/inv_webp.h"

using namespace std;
using namespace inv;

#ifndef JPEG_APP1
# define JPEG_APP1 (JPEG_APP0 + 1)
#endif
#ifndef JPEG_APP2
# define JPEG_APP2 (JPEG_APP0 + 2)
#endif

typedef struct {
  const uint8_t* data;
  size_t data_length;
  int seq;  // this segment's sequence number [1, 255] for use in reassembly.
} ICCPSegment;


void MetadataInit(Metadata* const metadata) {
  if (metadata == NULL) return;
  memset(metadata, 0, sizeof(*metadata));
}

void MetadataPayloadDelete(MetadataPayload* const payload) {
  if (payload == NULL) return;
  free(payload->bytes);
  payload->bytes = NULL;
  payload->size = 0;
}

void MetadataFree(Metadata* const metadata) {
  if (metadata == NULL) return;
  MetadataPayloadDelete(&metadata->exif);
  MetadataPayloadDelete(&metadata->iccp);
  MetadataPayloadDelete(&metadata->xmp);
}

int MetadataCopy(const char* metadata, size_t metadata_len,
                 MetadataPayload* const payload) {
  if (metadata == NULL || metadata_len == 0 || payload == NULL) return 0;
  payload->bytes = (uint8_t*)malloc(metadata_len);
  if (payload->bytes == NULL) return 0;
  payload->size = metadata_len;
  memcpy(payload->bytes, metadata, metadata_len);
  return 1;
}

static void PNGAPI error_function(png_structp png, png_const_charp error) {
  if (error != NULL) fprintf(stderr, "libpng error: %s\n", error);
  longjmp(png_jmpbuf(png), 1);
}


static void SaveMetadataMarkers(j_decompress_ptr dinfo) {
  const unsigned int max_marker_length = 0xffff;
  jpeg_save_markers(dinfo, JPEG_APP1, max_marker_length);  // Exif/XMP
  jpeg_save_markers(dinfo, JPEG_APP2, max_marker_length);  // ICC profile
}

static int CompareICCPSegments(const void* a, const void* b) {
  const ICCPSegment* s1 = (const ICCPSegment*)a;
  const ICCPSegment* s2 = (const ICCPSegment*)b;
  return s1->seq - s2->seq;
}

// Extract ICC profile segments from the marker list in 'dinfo', reassembling
// and storing them in 'iccp'.
// Returns true on success and false for memory errors and corrupt profiles.
static int StoreICCP(j_decompress_ptr dinfo, MetadataPayload* const iccp) {
  // ICC.1:2010-12 (4.3.0.0) Annex B.4 Embedding ICC Profiles in JPEG files
  static const char kICCPSignature[] = "ICC_PROFILE";
  static const size_t kICCPSignatureLength = 12;  // signature includes '\0'
  static const size_t kICCPSkipLength = 14;  // signature + seq & count
  int expected_count = 0;
  int actual_count = 0;
  int seq_max = 0;
  size_t total_size = 0;
  ICCPSegment iccp_segments[255];
  jpeg_saved_marker_ptr marker;

  memset(iccp_segments, 0, sizeof(iccp_segments));
  for (marker = dinfo->marker_list; marker != NULL; marker = marker->next) {
    if (marker->marker == JPEG_APP2 &&
        marker->data_length > kICCPSkipLength &&
        !memcmp(marker->data, kICCPSignature, kICCPSignatureLength)) {
      // ICC_PROFILE\0<seq><count>; 'seq' starts at 1.
      const int seq = marker->data[kICCPSignatureLength];
      const int count = marker->data[kICCPSignatureLength + 1];
      const size_t segment_size = marker->data_length - kICCPSkipLength;
      ICCPSegment* segment;

      if (segment_size == 0 || count == 0 || seq == 0) {
        fprintf(stderr, "[ICCP] size (%d) / count (%d) / sequence number (%d)"
                        " cannot be 0!\n",
                (int)segment_size, seq, count);
        return 0;
      }

      if (expected_count == 0) {
        expected_count = count;
      } else if (expected_count != count) {
        fprintf(stderr, "[ICCP] Inconsistent segment count (%d / %d)!\n",
                expected_count, count);
        return 0;
      }

      segment = iccp_segments + seq - 1;
      if (segment->data_length != 0) {
        fprintf(stderr, "[ICCP] Duplicate segment number (%d)!\n" , seq);
        return 0;
      }

      segment->data = marker->data + kICCPSkipLength;
      segment->data_length = segment_size;
      segment->seq = seq;
      total_size += segment_size;
      if (seq > seq_max) seq_max = seq;
      ++actual_count;
    }
  }

  if (actual_count == 0) return 1;
  if (seq_max != actual_count) {
    fprintf(stderr, "[ICCP] Discontinuous segments, expected: %d actual: %d!\n",
            actual_count, seq_max);
    return 0;
  }
  if (expected_count != actual_count) {
    fprintf(stderr, "[ICCP] Segment count: %d does not match expected: %d!\n",
            actual_count, expected_count);
    return 0;
  }

  // The segments may appear out of order in the file, sort them based on
  // sequence number before assembling the payload.
  qsort(iccp_segments, actual_count, sizeof(*iccp_segments),
        CompareICCPSegments);

  iccp->bytes = (uint8_t*)malloc(total_size);
  if (iccp->bytes == NULL) return 0;
  iccp->size = total_size;

  {
    int i;
    size_t offset = 0;
    for (i = 0; i < seq_max; ++i) {
      memcpy(iccp->bytes + offset,
             iccp_segments[i].data, iccp_segments[i].data_length);
      offset += iccp_segments[i].data_length;
    }
  }
  return 1;
}

// Returns true on success and false for memory errors and corrupt profiles.
// The caller must use MetadataFree() on 'metadata' in all cases.
static int ExtractMetadataFromJPEG(j_decompress_ptr dinfo,
                                   Metadata* const metadata) {
  static const struct {
    int marker;
    const char* signature;
    size_t signature_length;
    size_t storage_offset;
  } kJPEGMetadataMap[] = {
    // Exif 2.2 Section 4.7.2 Interoperability Structure of APP1 ...
    { JPEG_APP1, "Exif\0",                        6, METADATA_OFFSET(exif) },
    // XMP Specification Part 3 Section 3 Embedding XMP Metadata ... #JPEG
    // TODO(jzern) Add support for 'ExtendedXMP'
    { JPEG_APP1, "http://ns.adobe.com/xap/1.0/", 29, METADATA_OFFSET(xmp) },
    { 0, NULL, 0, 0 },
  };
  jpeg_saved_marker_ptr marker;
  // Treat ICC profiles separately as they may be segmented and out of order.
  if (!StoreICCP(dinfo, &metadata->iccp)) return 0;

  for (marker = dinfo->marker_list; marker != NULL; marker = marker->next) {
    int i;
    for (i = 0; kJPEGMetadataMap[i].marker != 0; ++i) {
      if (marker->marker == kJPEGMetadataMap[i].marker &&
          marker->data_length > kJPEGMetadataMap[i].signature_length &&
          !memcmp(marker->data, kJPEGMetadataMap[i].signature,
                  kJPEGMetadataMap[i].signature_length)) {
        MetadataPayload* const payload =
            (MetadataPayload*)((uint8_t*)metadata +
                               kJPEGMetadataMap[i].storage_offset);

        if (payload->bytes == NULL) {
          const char* marker_data = (const char*)marker->data +
                                    kJPEGMetadataMap[i].signature_length;
          const size_t marker_data_length =
              marker->data_length - kJPEGMetadataMap[i].signature_length;
          if (!MetadataCopy(marker_data, marker_data_length, payload)) return 0;
        } else {
          fprintf(stderr, "Ignoring additional '%s' marker\n",
                  kJPEGMetadataMap[i].signature);
        }
      }
    }
  }
  return 1;
}

#undef JPEG_APP1
#undef JPEG_APP2

// -----------------------------------------------------------------------------
// JPEG decoding

struct my_error_mgr {
  struct jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;
};

static void my_error_exit(j_common_ptr dinfo) {
  struct my_error_mgr* myerr = (struct my_error_mgr*)dinfo->err;
  dinfo->err->output_message(dinfo);
  longjmp(myerr->setjmp_buffer, 1);
}

int ReadJPEG(FILE* in_file, WebPPicture* const pic, Metadata* const metadata) {
  int ok = 0;
  int stride, width, height;
  volatile struct jpeg_decompress_struct dinfo;
  struct my_error_mgr jerr;
  uint8_t* volatile rgb = NULL;
  JSAMPROW buffer[1];

  memset((j_decompress_ptr)&dinfo, 0, sizeof(dinfo));   // for setjmp sanity
  dinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;

  if (setjmp(jerr.setjmp_buffer)) {
 Error:
    MetadataFree(metadata);
    jpeg_destroy_decompress((j_decompress_ptr)&dinfo);
    goto End;
  }

  jpeg_create_decompress((j_decompress_ptr)&dinfo);
  jpeg_stdio_src((j_decompress_ptr)&dinfo, in_file);
  if (metadata != NULL) SaveMetadataMarkers((j_decompress_ptr)&dinfo);
  jpeg_read_header((j_decompress_ptr)&dinfo, TRUE);

  dinfo.out_color_space = JCS_RGB;
  dinfo.do_fancy_upsampling = TRUE;

  jpeg_start_decompress((j_decompress_ptr)&dinfo);

  if (dinfo.output_components != 3) {
    goto Error;
  }

  width = dinfo.output_width;
  height = dinfo.output_height;
  stride = dinfo.output_width * dinfo.output_components * sizeof(*rgb);

  rgb = (uint8_t*)malloc(stride * height);
  if (rgb == NULL) {
    goto End;
  }
  buffer[0] = (JSAMPLE*)rgb;

  while (dinfo.output_scanline < dinfo.output_height) {
    if (jpeg_read_scanlines((j_decompress_ptr)&dinfo, buffer, 1) != 1) {
      goto End;
    }
    buffer[0] += stride;
  }

  if (metadata != NULL) {
    ok = ExtractMetadataFromJPEG((j_decompress_ptr)&dinfo, metadata);
    if (!ok) {
      fprintf(stderr, "Error extracting JPEG metadata!\n");
      goto Error;
    }
  }

  jpeg_finish_decompress((j_decompress_ptr)&dinfo);
  jpeg_destroy_decompress((j_decompress_ptr)&dinfo);

  // WebP conversion.
  pic->width = width;
  pic->height = height;
  pic->use_argb = 1;      // store raw RGB samples
  ok = WebPPictureImportRGB(pic, rgb, stride);
  if (!ok) goto Error;

 End:
  free(rgb);
  return ok;
}

// Converts the NULL terminated 'hexstring' which contains 2-byte character
// representations of hex values to raw data.
// 'hexstring' may contain values consisting of [A-F][a-f][0-9] in pairs,
// e.g., 7af2..., separated by any number of newlines.
// 'expected_length' is the anticipated processed size.
// On success the raw buffer is returned with its length equivalent to
// 'expected_length'. NULL is returned if the processed length is less than
// 'expected_length' or any character aside from those above is encountered.
// The returned buffer must be freed by the caller.
static uint8_t* HexStringToBytes(const char* hexstring,
                                 size_t expected_length) {
  const char* src = hexstring;
  size_t actual_length = 0;
  uint8_t* const raw_data = (uint8_t*)malloc(expected_length);
  uint8_t* dst;

  if (raw_data == NULL) return NULL;

  for (dst = raw_data; actual_length < expected_length && *src != '\0'; ++src) {
    char* end;
    char val[3];
    if (*src == '\n') continue;
    val[0] = *src++;
    val[1] = *src;
    val[2] = '\0';
    *dst++ = (uint8_t)strtol(val, &end, 16);
    if (end != val + 2) break;
    ++actual_length;
  }

  if (actual_length != expected_length) {
    free(raw_data);
    return NULL;
  }
  return raw_data;
}

static int ProcessRawProfile(const char* profile, size_t profile_len,
                             MetadataPayload* const payload) {
  const char* src = profile;
  char* end;
  int expected_length;

  if (profile == NULL || profile_len == 0) return 0;

  // ImageMagick formats 'raw profiles' as
  // '\n<name>\n<length>(%8lu)\n<hex payload>\n'.
  if (*src != '\n') {
    fprintf(stderr, "Malformed raw profile, expected '\\n' got '\\x%.2X'\n",
            *src);
    return 0;
  }
  ++src;
  // skip the profile name and extract the length.
  while (*src != '\0' && *src++ != '\n') {}
  expected_length = (int)strtol(src, &end, 10);
  if (*end != '\n') {
    fprintf(stderr, "Malformed raw profile, expected '\\n' got '\\x%.2X'\n",
            *end);
    return 0;
  }
  ++end;

  // 'end' now points to the profile payload.
  payload->bytes = HexStringToBytes(end, expected_length);
  if (payload->bytes == NULL) return 0;
  payload->size = expected_length;
  return 1;
}

static const struct {
  const char* name;
  int (*process)(const char* profile, size_t profile_len,
                 MetadataPayload* const payload);
  size_t storage_offset;
} kPNGMetadataMap[] = {
  // http://www.sno.phy.queensu.ca/~phil/exiftool/TagNames/PNG.html#TextualData
  // See also: ExifTool on CPAN.
  { "Raw profile type exif", ProcessRawProfile, METADATA_OFFSET(exif) },
  { "Raw profile type xmp",  ProcessRawProfile, METADATA_OFFSET(xmp) },
  // Exiftool puts exif data in APP1 chunk, too.
  { "Raw profile type APP1", ProcessRawProfile, METADATA_OFFSET(exif) },
  // XMP Specification Part 3, Section 3 #PNG
  { "XML:com.adobe.xmp",     MetadataCopy,      METADATA_OFFSET(xmp) },
  { NULL, NULL, 0 },
};

// Looks for metadata at both the beginning and end of the PNG file, giving
// preference to the head.
// Returns true on success. The caller must use MetadataFree() on 'metadata' in
// all cases.
static int ExtractMetadataFromPNG(png_structp png,
                                  png_infop const head_info,
                                  png_infop const end_info,
                                  Metadata* const metadata) {
  int p;

  for (p = 0; p < 2; ++p)  {
    png_infop const info = (p == 0) ? head_info : end_info;
    png_textp text = NULL;
    const int num = png_get_text(png, info, &text, NULL);
    int i;
    // Look for EXIF / XMP metadata.
    for (i = 0; i < num; ++i, ++text) {
      int j;
      for (j = 0; kPNGMetadataMap[j].name != NULL; ++j) {
        if (!strcmp(text->key, kPNGMetadataMap[j].name)) {
          MetadataPayload* const payload =
              (MetadataPayload*)((uint8_t*)metadata +
                                 kPNGMetadataMap[j].storage_offset);
          png_size_t text_length;
          switch (text->compression) {
#ifdef PNG_iTXt_SUPPORTED
            case PNG_ITXT_COMPRESSION_NONE:
            case PNG_ITXT_COMPRESSION_zTXt:
              text_length = text->itxt_length;
              break;
#endif
            case PNG_TEXT_COMPRESSION_NONE:
            case PNG_TEXT_COMPRESSION_zTXt:
            default:
              text_length = text->text_length;
              break;
          }
          if (payload->bytes != NULL) {
            fprintf(stderr, "Ignoring additional '%s'\n", text->key);
          } else if (!kPNGMetadataMap[j].process(text->text, text_length,
                                                 payload)) {
            fprintf(stderr, "Failed to process: '%s'\n", text->key);
            return 0;
          }
          break;
        }
      }
    }
    // Look for an ICC profile.
    {
      png_charp name;
      int comp_type;
#if ((PNG_LIBPNG_VER_MAJOR << 8) | PNG_LIBPNG_VER_MINOR << 0) < \
    ((1 << 8) | (5 << 0))
      png_charp profile;
#else  // >= libpng 1.5.0
      png_bytep profile;
#endif
      png_uint_32 len;

      if (png_get_iCCP(png, info,
                       &name, &comp_type, &profile, &len) == PNG_INFO_iCCP) {
        if (!MetadataCopy((const char*)profile, len, &metadata->iccp)) return 0;
      }
    }
  }

  return 1;
}

int ReadPNG(FILE* in_file, WebPPicture* const pic, int keep_alpha,
            Metadata* const metadata) {
  volatile png_structp png;
  volatile png_infop info = NULL;
  volatile png_infop end_info = NULL;
  int color_type, bit_depth, interlaced;
  int has_alpha;
  int num_passes;
  int p;
  int ok = 0;
  png_uint_32 width, height, y;
  int stride;
  uint8_t* volatile rgb = NULL;

  png = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
  if (png == NULL) {
    goto End;
  }

  png_set_error_fn(png, 0, error_function, NULL);
  if (setjmp(png_jmpbuf(png))) {
 Error:
    MetadataFree(metadata);
    goto End;
  }

  info = png_create_info_struct(png);
  if (info == NULL) goto Error;
  end_info = png_create_info_struct(png);
  if (end_info == NULL) goto Error;

  png_init_io(png, in_file);
  png_read_info(png, info);
  if (!png_get_IHDR(png, info,
                    &width, &height, &bit_depth, &color_type, &interlaced,
                    NULL, NULL)) goto Error;

  png_set_strip_16(png);
  png_set_packing(png);
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_palette_to_rgb(png);
  }
  if (color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    if (bit_depth < 8) {
      png_set_expand_gray_1_2_4_to_8(png);
    }
    png_set_gray_to_rgb(png);
  }
  if (png_get_valid(png, info, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(png);
    has_alpha = 1;
  } else {
    has_alpha = !!(color_type & PNG_COLOR_MASK_ALPHA);
  }

  if (!keep_alpha) {
    png_set_strip_alpha(png);
    has_alpha = 0;
  }

  num_passes = png_set_interlace_handling(png);
  png_read_update_info(png, info);
  stride = (has_alpha ? 4 : 3) * width * sizeof(*rgb);
  rgb = (uint8_t*)malloc(stride * height);
  if (rgb == NULL) goto Error;
  for (p = 0; p < num_passes; ++p) {
    for (y = 0; y < height; ++y) {
      png_bytep row = (png_bytep)(rgb + y * stride);
      png_read_rows(png, &row, NULL, 1);
    }
  }
  png_read_end(png, end_info);

  if (metadata != NULL &&
      !ExtractMetadataFromPNG(png, info, end_info, metadata)) {
    fprintf(stderr, "Error extracting PNG metadata!\n");
    goto Error;
  }

  pic->width = width;
  pic->height = height;
  pic->use_argb = 1;
  ok = has_alpha ? WebPPictureImportRGBA(pic, rgb, stride)
                 : WebPPictureImportRGB(pic, rgb, stride);

  if (!ok) {
    goto Error;
  }

 End:
  if (png != NULL) {
    png_destroy_read_struct((png_structpp)&png,
                            (png_infopp)&info, (png_infopp)&end_info);
  }
  free(rgb);
  return ok;
}


InputFileFormat INV_Webp::GetImageType(FILE* in_file) {
  InputFileFormat format = UNSUPPORTED;
  uint32_t magic1, magic2;
  uint8_t buf[12];

  if ((fread(&buf[0], 12, 1, in_file) != 1) ||
      (fseek(in_file, 0, SEEK_SET) != 0)) {
    return format;
  }

  magic1 = ((uint32_t)buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
  magic2 = ((uint32_t)buf[8] << 24) | (buf[9] << 16) | (buf[10] << 8) | buf[11];
  if (magic1 == 0x89504E47U) {
    format = PNG_;
  } else if (magic1 >= 0xFFD8FF00U && magic1 <= 0xFFD8FFFFU) {
    format = JPEG_;
  } else if (magic1 == 0x49492A00 || magic1 == 0x4D4D002A) {
    format = TIFF_;
  } else if (magic1 == 0x52494646 && magic2 == 0x57454250) {
    format = WEBP_;
  }
  return format;
}

int INV_Webp::MyWriter(const uint8_t* data, size_t data_size,
                    const WebPPicture* const pic) {
  FILE* const out = (FILE*)pic->custom_ptr;
  return data_size ? (fwrite(data, data_size, 1, out) == 1) : 1;
}

int INV_Webp::ReadPicture(const char* const filename, WebPPicture* const pic,
                       int keep_alpha, Metadata* const metadata) {
  int ok = 0;
  FILE* in_file = fopen(filename, "rb");
  if (in_file == NULL) {
    fprintf(stderr, "Error! Cannot open input file '%s'\n", filename);
    return ok;
  }

  pic->user_data = (void*)in_file;

  const InputFileFormat format = GetImageType(in_file);
  if (format == PNG_) {
    ok = ReadPNG(in_file, pic, keep_alpha, metadata);
  } else if (format == JPEG_) {
    ok = ReadJPEG(in_file, pic, metadata);
  }

  if (!ok) {
    fprintf(stderr, "Error! Could not process file %s\n", filename);
  }

  fclose(in_file);
  return ok;
}

string INV_Webp::pic2webp(const char *filename)
{
	WebPPicture picture;
	WebPConfig config;
	Metadata metadata;
	WebPAuxStats stats;

	MetadataInit(&metadata);
	WebPConfigInit(&config);
	WebPPictureInit(&picture);
	int ok = 0;
	FILE *out = NULL;
	string fileto;
	do
	{
		config.quality = 80;

		int keep_alpha = 1;

		fileto = filename+string(".webp");

		out = fopen(fileto.c_str(), "wb");

		picture.writer = MyWriter;
    picture.custom_ptr = (void*)out;
    picture.stats = &stats;

		ok = ReadPicture(filename, &picture, keep_alpha, &metadata);
		if(!ok){
			break;
		}

		ok = WebPEncode(&config, &picture);
	}while(0);

	MetadataFree(&metadata);
	WebPPictureFree(&picture);

	if(out != NULL){
		fclose(out);
	}

	string value;
	if(ok)
	{
		ifstream ifs(fileto.c_str());
		value = string(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>());
	}

	remove(fileto.c_str());
	
  return value;
}

