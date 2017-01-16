#ifndef __INV_IMAGE_H
#define __INV_IMAGE_H

#include <map>
#include <vector>
#include <string>
#include <sstream>

#include <string.h>
#include "opencv/cv.h"

using namespace std;

namespace inv
{

enum ResizeMethod {
  kResizeWithNone = 0,
  kResizeWithOpenCV = 1,
  kResizeWithMagic = 2
};

class INV_Image {
public:
  INV_Image();
  ~INV_Image();
  // quality [1, 100]
  static bool CompressImage(int quality, std::string *content, int *height, int *width, int *content_type);
  static bool ResizeImage(int height, int width, int image_type, std::string *image);
  static bool ResizeFaceImage(int height, int width, int image_type,std::string *image,int face_ordinate, int face_height, bool is_portrait,int face_abscissa);
  static bool ResizeImageWithImageMagic(int height, int width, int image_type, std::string *image);
  /**
   * @brief 裁剪图片
   *        无默认参数:从左上角开始裁剪
   *        默认参数只提供face_ordinate:根据人脸区域裁剪(人脸放在图片上半部),有缩放
   *        默认参数全部提供:人脸区域居中裁剪,无缩放
   *
   *
   * @param height [IN]裁剪后的高度
   * @param width [IN]裁剪后的宽度
   * @param file_name [OUT]裁剪后存放路径
   * @param image [IN]待裁剪的图片内容
   * @param face_ordinate [IN]人脸区域上沿纵坐标
   * @param face_height [IN]人脸区域高度
   * @param is_portrait [IN]是否是肖像照(头像高度占照片高度10分之1以上)
   *
   * @return 
   */
  static bool ResizeImageWithOpenCV(int height, int width, const std::string &file_name, std::string *image, int face_ordinate = 0, int face_height = 0, bool is_portrait = false, int face_abscissa = 0);
  /**
   * @brief 使用特征级联检测人脸信息
   *
   * @param file_content [IN]图片内容
   * @param cascade [IN]特征级联文件的路径
   * @param face_ordinate [OUT]头像上沿纵坐标
   * @param face_height [OUT]头像区高度
   * @param is_portrait [OUT]是否是肖像照(头像高度占照片高度10分之1以上)
   * @param face_abscissa [OUT]头像左沿纵坐标
   * @param face_width [OUT]头像区宽度
   *
   * @return true检测到了人脸   false没有检测到人脸
   */
  static bool FaceDetect(const std::string& file_content, const std::string& cascade, int *face_ordinate, int *face_height, bool *is_portrait, int *face_abscissa, int *face_width);
  /**
   * @brief 使用两个级联特征文件检测人脸,提高准确度,其他参数同上
   */
  static bool FaceDetect(const std::string& file_content, const std::string& cascade1, const std::string& cascade2, int *face_ordinate, int *face_height, bool *is_portrait, int *face_abscissa, int *face_width);
  static ResizeMethod JudgeResizeMethod(int height, int width, int image_type, std::string *image);
private:
  static std::string GetTmpFilename(const std::string &suffix);
  static bool SaveImageToFile(const std::string &image, const std::string &file_name);
  static bool ReadImageFromFile(const std::string &file_name, std::string *image);
  static void FixRequestWH(int original_height, int original_width, int *height, int *width);
  static bool Overlap(const cv::Rect& r1, const cv::Rect& r2);
  static vector<cv::Rect> Intersetcion(const std::vector<cv::Rect>& rects3, const std::vector<cv::Rect>& rects2);
};

}

#endif
