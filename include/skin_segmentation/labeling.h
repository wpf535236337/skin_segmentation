#ifndef _SKINSEG_LABELING_H_
#define _SKINSEG_LABELING_H_

#include "rosbag/bag.h"
#include "sensor_msgs/Image.h"

#include "skin_segmentation/projection.h"

namespace skinseg {
class Labeling {
 public:
  // Takes in an open bag to write to.
  Labeling(const Projection& projection, rosbag::Bag* output_bag);

  void Process(const sensor_msgs::Image::ConstPtr& rgb,
               const sensor_msgs::Image::ConstPtr& depth,
               const sensor_msgs::Image::ConstPtr& thermal);

  // If debug is true, then wait for a keypress between each image.
  void set_debug(bool debug);

 private:
  Projection projection_;
  rosbag::Bag* output_bag_;
  bool debug_;
};
}  // namespace skinseg

#endif  // _SKINSEG_LABELING_H_
