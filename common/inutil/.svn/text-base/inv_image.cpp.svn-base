#include <unistd.h>

#include "util/inv_image.h"
#include "util/inv_util.h"
#include "util/inv_file.h"
#include "log/inv_log.h"

#include "Magick++.h"
#include "opencv2/opencv.hpp"

using namespace inv;
using namespace cv;

#define kGIFCompress 1
#define kJPGCompress 2


bool INV_Image::CompressImage(int quality, std::string *content,
                                   int *height, int *width, int *content_type) {
  *content_type = (strncmp(content->c_str(), "GIF", 3) == 0 ? kGIFCompress : kJPGCompress);
  try {
    Magick::Blob ori_blob(content->data(), content->size());
    Magick::Image image(ori_blob);
    if (*content_type == kJPGCompress) {
      image.quality(quality);
      image.compressType(Magick::JPEGCompression);

      Magick::Blob my_blob;
      image.write(&my_blob);
      content->assign((const char*)my_blob.data(), my_blob.length());
    }
    *height = image.baseRows();
    *width = image.baseColumns();
    return true;
  } catch (Magick::Exception &ex) {
    FDLOG("error") << "CompressImage:" << ex.what() << endl;
    return false;
  }
}

std::string INV_Image::GetTmpFilename(const std::string &suffix) {
  int tid = getpid();
  int pid = pthread_self();

  return "/dev/shm/image_server_tmp_" + INV_Util::tostr(tid) + "_" + INV_Util::tostr(pid) + "." + suffix;
  //return base::StringPrintf("/dev/shm/image_server_tmp_%d_%d.", tid, pid) + suffix;
}

bool INV_Image::SaveImageToFile(const std::string &image, const std::string &file_name) {
  INV_File::save2file(file_name, image.c_str(), image.size());
  return true;
}

bool INV_Image::ReadImageFromFile(const std::string &file_name, std::string *image) {
  image->clear();

  try { 
    *image = INV_File::load2str(file_name);
    return true;
  }
  catch (std::exception& e)
  {
    FDLOG("error") << "load file:" << file_name << " failed, exception:" << e.what() << ", err:" << strerror(errno) << endl;
  }
  catch (...)
  {
    FDLOG("error") << "load file:" << file_name << " failed, unknow exception, err:" << strerror(errno) << endl;
  }

  return false;
}

bool INV_Image::ResizeImageWithOpenCV(int height, int width,
        const std::string &file_name, std::string *image, 
        int face_ordinate, int face_height, bool is_portrait, int face_abscissa) {
  if (!SaveImageToFile(*image, file_name)) {
    return false;
  }
  IplImage* input_image = cvLoadImage(file_name.c_str());
  if (input_image == NULL) {
    return false;
  }
  FixRequestWH(input_image->height, input_image->width, &height, &width);
  int new_height = input_image->height;
  int new_width = input_image->width;
  if (height > new_height) {
    height = new_height;
  }
  if (width > new_width) {
    width = new_width;
  }
  // 按请求的长宽比调整原图片的ROI
  if (new_height * width > new_width * height) {
    new_height = new_width * height / (width > 0 ? width : 1);
  } else {
    new_width = new_height * width / (height > 0 ? height : 1);
  }

  if(new_width < 1 || new_height < 1) 
  {
      return false;
  }

  CvRect roi;
  if (is_portrait)
  {
      // 肖像照不缩放,按原请求高度裁剪人脸区
      roi.x = face_abscissa+width<input_image->width ? face_abscissa : std::max(0, input_image->width-width);
      roi.y = height>2*face_ordinate+face_height ? 0 : std::min(input_image->height-height, face_ordinate+face_height/2-height/2);
      roi.width = std::min(input_image->width, width);
      roi.height = roi.y+height<input_image->height ? height : input_image->height-roi.y;
      cvSetImageROI(input_image, roi);
      IplImage* image_final = cvCreateImage(cvSize(roi.width, roi.height), input_image->depth,
              input_image->nChannels);
      cvCopy(input_image, image_final);
      cvSaveImage(file_name.c_str(), image_final);
      cvReleaseImage(&image_final);
  }
  else
  {
      // 人脸上方的空白区域高度最多只占k分之1
      const int k = 5;
      roi.x = 0;
      roi.y = std::min(input_image->height-new_height, 
              face_ordinate*k<new_height ? 0 : (k*face_ordinate-new_height)/(k-1));
      roi.width = new_width;
      roi.height = new_height;
      cvSetImageROI(input_image, roi);
      IplImage* image_after_cut = cvCreateImage(cvSize(roi.width, roi.height), input_image->depth,
              input_image->nChannels);
      cvCopy(input_image, image_after_cut);

      IplImage* image_final = cvCreateImage(cvSize(width, height), input_image->depth,
              input_image->nChannels);
      cvResize(image_after_cut, image_final);
      cvSaveImage(file_name.c_str(), image_final);
      cvReleaseImage(&image_after_cut);
      cvReleaseImage(&image_final);
  }
  cvReleaseImage(&input_image);

  if (!ReadImageFromFile(file_name, image)) {
    return false;
  }
  return true;
}

bool INV_Image::FaceDetect(const std::string& file_content, const std::string& cascade, 
        int *face_ordinate, int *face_height, bool *is_portrait, int *face_abscissa, int *face_width)
{
    static CascadeClassifier face_cascade;
    // 首次进入时加载分类器特征级联
    if (face_cascade.empty())
    {
        if (!face_cascade.load(cascade))
        {
            return false;
        }
    }
    // 加载源图片
    vector<char> ia;
    ia.resize(file_content.size());
    ia.assign(file_content.begin(), file_content.end());
    Mat image = imdecode(ia, CV_LOAD_IMAGE_COLOR);
    // 源图片预处理
    Mat image_gray;
    cvtColor(image, image_gray, CV_BGR2GRAY);
    equalizeHist(image_gray, image_gray);
    // 检测人脸
    vector<Rect> faces;
    // 调整参数会有不同的检测结果
    face_cascade.detectMultiScale(image_gray,
            faces, 1.1, 2);//, 0/*|CV_HAAR_SCALE_IMAGE*/, Size(30, 30));
    if (!faces.size()) return false;
    // 计算人脸区域
    // 只返回检测到的最大的那张脸的区域
    size_t biggest = 0;
    for (size_t i = 0; i < faces.size(); i++)
    {
        if (faces[i].height*faces[i].width > faces[biggest].height*faces[biggest].width)
        {
            biggest = i;
        }
    }

    if (face_ordinate) *face_ordinate = faces[biggest].y;
    if (face_abscissa) *face_abscissa = std::max(faces[biggest].x-faces[biggest].width/2, 0); // 左移半个脸的宽度
    if (face_height) *face_height = faces[biggest].height;
    if (face_width) *face_width = faces[biggest].width;
    if (is_portrait) *is_portrait = 10*faces[biggest].height>image.rows;
    return true;
}

bool INV_Image::Overlap(const Rect& r1, const Rect& r2)
{
    return (r1.x+r1.width>=r2.x) && (r1.y+r1.height>=r2.y) && (r2.x+r2.width>=r1.x) && (r2.y+r2.height>=r1.y);
}

vector<Rect> INV_Image::Intersetcion(const vector<Rect>& rects1, const vector<Rect>& rects2)
{
    vector<Rect> rects;
    for (size_t i = 0; i < rects1.size(); i++)
    {
        for (size_t j = 0; j < rects2.size(); j++)
        {
            if (Overlap(rects1[i], rects2[j]))
            {
                rects.push_back(rects1[i]);
                break;
            }
        }
    }
    return rects;
}

bool INV_Image::FaceDetect(const std::string& file_content, const std::string& cascade1, const std::string& cascade2,
        int *face_ordinate, int *face_height, bool *is_portrait, int *face_abscissa, int *face_width)
{
    static CascadeClassifier face_cascade1;
    static CascadeClassifier face_cascade2;
    // 首次进入时加载分类器特征级联
    if (face_cascade1.empty())
    {
        if (!face_cascade1.load(cascade1))
        {
            return false;
        }
    }
    if (face_cascade2.empty())
    {
        if (!face_cascade2.load(cascade2))
        {
            return false;
        }
    }
    // 加载源图片
    vector<char> ia;
    ia.resize(file_content.size());
    ia.assign(file_content.begin(), file_content.end());
    Mat image = imdecode(ia, CV_LOAD_IMAGE_COLOR);
    // 源图片预处理
    Mat image_gray;
    cvtColor(image, image_gray, CV_BGR2GRAY);
    equalizeHist(image_gray, image_gray);
    // 检测人脸
    vector<Rect> faces1;
    vector<Rect> faces2;
    // 调整参数会有不同的检测结果
    face_cascade1.detectMultiScale(image_gray,
            faces1, 1.1, 2);//, 0/*|CV_HAAR_SCALE_IMAGE*/, Size(30, 30));
    face_cascade2.detectMultiScale(image_gray,
            faces2, 1.1, 2);//, 0/*|CV_HAAR_SCALE_IMAGE*/, Size(30, 30));
    // 两个检测结果取交集
    vector<Rect> faces = Intersetcion(faces1, faces2);
    if (!faces.size()) return false;
    // 计算人脸区域
    // 只返回检测到的最大的那张脸的区域
    size_t biggest = 0;
    for (size_t i = 0; i < faces.size(); i++)
    {
        if (faces[i].height*faces[i].width > faces[biggest].height*faces[biggest].width)
        {
            biggest = i;
        }
    }

    if (face_ordinate) *face_ordinate = faces[biggest].y;
    if (face_abscissa) *face_abscissa = std::max(faces[biggest].x-faces[biggest].width/2, 0); // 左移半个脸的宽度
    if (face_height) *face_height = faces[biggest].height;
    if (face_width) *face_width = faces[biggest].width;
    if (is_portrait) *is_portrait = 10*faces[biggest].height>image.rows;
    return true;
}

bool INV_Image::ResizeImageWithImageMagic(int height, int width, int image_type, std::string *image) {
  int64_t load_time = 0;
  int64_t resize_and_compress_time = 0;
  try {
    int64_t begin_timestamp = INV_Util::now2ms();
    Magick::Blob ori_blob(image->data(), image->size());
    Magick::Image url_image(ori_blob);
    int64_t load_finish_timestamp = INV_Util::now2ms();
    load_time = load_finish_timestamp - begin_timestamp;

    int original_height = url_image.baseRows();
    int original_width = url_image.baseColumns();

    FixRequestWH(original_height, original_width, &height, &width);
    int new_height = 0;
    int new_width = 0;
    if (original_height * width > original_width * height) {
      new_height = original_width * height / (width > 0 ? width : 0);
    } else {
      new_width = original_height * width / (height > 0 ? height : 0);
    }
    if (new_height != original_height || new_width != original_width) {
      Magick::Geometry newsize(new_width, new_height, 0, 0);
      url_image.crop(newsize);
    }


    std::string size_str = INV_Util::tostr(width) + "x" + INV_Util::tostr(height);
    Magick::Geometry newsize(size_str);
    newsize.greater(true);
    newsize.less(false);
    url_image.quality(85);
    url_image.resize(newsize);
    url_image.compressType(Magick::JPEGCompression);

    Magick::Blob my_blob;
    url_image.write(&my_blob);
    image->assign((const char*)my_blob.data(), my_blob.length());
    int64_t end_timestamp = INV_Util::now2ms();
    resize_and_compress_time = end_timestamp - load_finish_timestamp;
    
    return true;
  } catch (Magick::Exception) {
    return false;
  }
}

bool INV_Image::ResizeFaceImage(int height, int width, int image_type,std::string *image,int face_ordinate, int face_height, bool is_portrait,int face_abscissa) {
  
    std::string suffix = (image_type == kJPGCompress ? "jpg" : "gif");
    std::string tmp_file_name = GetTmpFilename(suffix);
    bool success = ResizeImageWithOpenCV(height, width,tmp_file_name,image,face_ordinate,face_height,is_portrait,face_abscissa);
    INV_File::removeFile(tmp_file_name, false);
    return success;
  //return ResizeImageWithImageMagic(height, width, image_type, image);
}

bool INV_Image::ResizeImage(int height, int width, int image_type, std::string *image) {

  ResizeMethod method = JudgeResizeMethod(height, width, image_type, image);
  if (method == kResizeWithNone) { return true; }
  if (method == kResizeWithMagic) { return ResizeImageWithImageMagic(height, width, image_type, image); }
  if (method == kResizeWithOpenCV) {
    std::string suffix = (image_type == kJPGCompress ? "jpg" : "gif");
    std::string tmp_file_name = GetTmpFilename(suffix);
    bool success = ResizeImageWithOpenCV(height, width, tmp_file_name, image);
    INV_File::removeFile(tmp_file_name, false);
    return success;
  }
  return false;

  //return ResizeImageWithImageMagic(height, width, image_type, image);
}

ResizeMethod INV_Image::JudgeResizeMethod(int height, int width, int image_type, std::string *image) {
  if (height == 0 && width == 0) {
    return kResizeWithNone;
  }
  if (image_type != kJPGCompress || strncmp(image->c_str(), "GIF", 3) == 0 ||
      strncmp(image->c_str(), "BM", 2) == 0) {
    return kResizeWithMagic;
  }
  return kResizeWithOpenCV;
}

void INV_Image::FixRequestWH(int original_height, int original_width, int *height, int *width) {
  if (*height == 0) {
    if (*width > original_width) {
      *width = original_width;
    }
    *height = *width * original_height / std::max(original_width, 1);
  } else if (*width == 0) {
    if (*height > original_height) {
      *height = original_height;
    }
    *width = *height * original_width / std::max(original_height, 1);
  }
}

