#ifndef UTIL_HPP_
#define UTIL_HPP_

#include <k4a/k4a.hpp>
#include <opencv2/opencv.hpp>

cv::Mat get_mat(k4a::image& src, bool deep_copy = true) {
  assert(src.get_size() != 0);

  cv::Mat mat;
  const int32_t width = src.get_width_pixels();
  const int32_t height = src.get_height_pixels();

  const k4a_image_format_t format = src.get_format();
  switch (format) {
    case k4a_image_format_t::K4A_IMAGE_FORMAT_COLOR_MJPG: {
      // NOTE: this is slower than other formats.
      std::vector<uint8_t> buffer(src.get_buffer(),
                                  src.get_buffer() + src.get_size());
      mat = cv::imdecode(buffer, cv::IMREAD_ANYCOLOR);
      cv::cvtColor(mat, mat, cv::COLOR_BGR2BGRA);
      break;
    }
    case k4a_image_format_t::K4A_IMAGE_FORMAT_COLOR_NV12: {
      cv::Mat nv12 =
          cv::Mat(height + height / 2, width, CV_8UC1, src.get_buffer())
              .clone();
      cv::cvtColor(nv12, mat, cv::COLOR_YUV2BGRA_NV12);
      break;
    }
    case k4a_image_format_t::K4A_IMAGE_FORMAT_COLOR_YUY2: {
      cv::Mat yuy2 = cv::Mat(height, width, CV_8UC2, src.get_buffer()).clone();
      cv::cvtColor(yuy2, mat, cv::COLOR_YUV2BGRA_YUY2);
      break;
    }
    case k4a_image_format_t::K4A_IMAGE_FORMAT_COLOR_BGRA32: {
      mat = deep_copy
                ? cv::Mat(height, width, CV_8UC4, src.get_buffer()).clone()
                : cv::Mat(height, width, CV_8UC4, src.get_buffer());
      break;
    }
    case k4a_image_format_t::K4A_IMAGE_FORMAT_DEPTH16:
    case k4a_image_format_t::K4A_IMAGE_FORMAT_IR16: {
      mat = deep_copy ? cv::Mat(height, width, CV_16UC1,
                                reinterpret_cast<uint16_t*>(src.get_buffer()))
                            .clone()
                      : cv::Mat(height, width, CV_16UC1,
                                reinterpret_cast<uint16_t*>(src.get_buffer()));
      break;
    }
    case k4a_image_format_t::K4A_IMAGE_FORMAT_CUSTOM8: {
      mat = cv::Mat(height, width, CV_8UC1, src.get_buffer()).clone();
      break;
    }
    case k4a_image_format_t::K4A_IMAGE_FORMAT_CUSTOM: {
      // NOTE: This is opencv_viz module format (cv::viz::WCloud).
      const int16_t* buffer = reinterpret_cast<int16_t*>(src.get_buffer());
      mat = cv::Mat(height, width, CV_32FC3,
                    cv::Vec3f::all(std::numeric_limits<float>::quiet_NaN()));
      mat.forEach<cv::Vec3f>([&](cv::Vec3f& point, const int32_t* position) {
        const int32_t index = (position[0] * width + position[1]) * 3;
        point =
            cv::Vec3f(buffer[index + 0], buffer[index + 1], buffer[index + 2]);
      });
      break;
    }
    default:
      throw k4a::error("Failed to convert this format!");
      break;
  }

  return mat;
}

cv::Mat capture(k4a::device* device) {
  k4a::capture k4a_cap;
  device->get_capture(&k4a_cap);
  k4a::image depth = k4a_cap.get_depth_image();
  return get_mat(depth);
}

#endif  // UTIL_HPP_