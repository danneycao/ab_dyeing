#ifndef __INV_WEBP_H
#define __INV_WEBP_H

#include "webp/types.h"
#include "webp/encode.h"
#include <string>
#include <iostream>
#include <fstream>

typedef struct MetadataPayload {
  uint8_t* bytes;
  size_t size;
} MetadataPayload;

typedef struct Metadata {
  MetadataPayload exif;
  MetadataPayload iccp;
  MetadataPayload xmp;
} Metadata;

#define METADATA_OFFSET(x) offsetof(Metadata, x)

typedef enum {
  PNG_ = 0,
  JPEG_,
  TIFF_,  // 'TIFF' clashes with libtiff
  WEBP_,
  UNSUPPORTED
} InputFileFormat;

using namespace std;

namespace inv
{

class INV_Webp
{
public:

    static InputFileFormat GetImageType(FILE* in_file);
    static int MyWriter(const uint8_t* data, size_t data_size, const WebPPicture* const pic);
    static int ReadPicture(const char* const filename, WebPPicture* const pic,
                       int keep_alpha, Metadata* const metadata);



    static string pic2webp(const char *filename);
};

}

#endif