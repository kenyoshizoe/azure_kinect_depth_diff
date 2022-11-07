#include <k4a/k4a.hpp>
#include <opencv2/opencv.hpp>

#include "util.hpp"

int main() {
  // Check Azure Kinect is connected
  uint32_t count = k4a_device_get_installed_count();
  if (count == 0) {
    printf("No k4a devices attached!\n");
    return 1;
  }

  // SetUp Azure Kinect
  k4a::device device = k4a::device::open(0);

  k4a_device_configuration_t config;
  config.color_format = k4a_image_format_t::K4A_IMAGE_FORMAT_COLOR_MJPG;
  config.color_resolution = k4a_color_resolution_t::K4A_COLOR_RESOLUTION_1080P;
  config.depth_mode = k4a_depth_mode_t::K4A_DEPTH_MODE_NFOV_2X2BINNED;
  config.camera_fps = k4a_fps_t::K4A_FRAMES_PER_SECOND_30;
  config.synchronized_images_only = false;
  config.depth_delay_off_color_usec = 0;
  config.wired_sync_mode =
      k4a_wired_sync_mode_t::K4A_WIRED_SYNC_MODE_STANDALONE;
  config.subordinate_delay_off_master_usec = 0;
  device.start_cameras(&config);

  while (true) {
    cv::Mat cap = capture(&device);
    cv::imshow("azure kinect", cap);

    auto key = cv::waitKey(1);
    if (key == 'q') {
      break;
    }
  }

  device.close();
  return 0;
}
