#include <iostream>
#include <string>
#include <vector>

#include "message_filters/cache.h"
#include "message_filters/subscriber.h"
#include "message_filters/sync_policies/approximate_time.h"
#include "message_filters/synchronizer.h"
#include "ros/ros.h"
#include "rosbag/bag.h"
#include "rosbag/view.h"
#include "sensor_msgs/Image.h"

#include "skin_segmentation/calibration.h"

using sensor_msgs::Image;
typedef message_filters::sync_policies::ApproximateTime<Image, Image> MyPolicy;

static const char kRgbTopic[] = "/camera/rgb/image_rect_color";
static const char kThermalTopic[] = "/ici/ir_camera/image_normalized_rgb";

void PrintUsage() {
  std::cout << "Computes calibration between thermal and RGB cameras, using "
               "data from record_calibration_data.launch."
            << std::endl;
  std::cout << "Usage: rosrun skin_segmentation calibration "
               "~/calibration_images.bag"
            << std::endl;
}

int main(int argc, char** argv) {
  ros::init(argc, argv, "extrinsic_calibration");
  ros::NodeHandle nh;
  if (argc < 2) {
    PrintUsage();
    return 1;
  }

  skinseg::Calibration calibration;

  message_filters::Cache<Image> rgb_cache(20);
  message_filters::Cache<Image> thermal_cache(20);
  message_filters::Synchronizer<MyPolicy> sync(MyPolicy(10), rgb_cache,
                                               thermal_cache);
  sync.registerCallback(&skinseg::Calibration::AddImagePair, &calibration);

  rosbag::Bag bag;
  bag.open(argv[1], rosbag::bagmode::Read);
  std::vector<std::string> topics;
  topics.push_back(kRgbTopic);
  topics.push_back(kThermalTopic);
  rosbag::View view(bag, rosbag::TopicQuery(topics));
  size_t size = view.size();
  int i = 0;
  for (rosbag::View::const_iterator it = view.begin(); it != view.end(); ++it) {
    Image::ConstPtr image = it->instantiate<Image>();
    if (!image) {
      continue;
    }
    if (it->getTopic() == kRgbTopic) {
      rgb_cache.add(image);
    } else if (it->getTopic() == kThermalTopic) {
      thermal_cache.add(image);
    }
    if (++i % 100 == 0) {
      ROS_INFO("Processing image %d of %ld", i, size);
    }
  }
  bag.close();

  calibration.Run();

  return 0;
}