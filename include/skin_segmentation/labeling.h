#ifndef _SKINSEG_LABELING_H_
#define _SKINSEG_LABELING_H_

#include <vector_types.h>
#include <vector>

#include "pcl/PointIndices.h"
#include "pcl/point_cloud.h"
#include "pcl/point_types.h"
#include "ros/ros.h"
#include "rosbag/bag.h"
#include "sensor_msgs/CameraInfo.h"
#include "sensor_msgs/Image.h"

#include "skin_segmentation/nerf.h"
#include "skin_segmentation/projection.h"

namespace skinseg {
static const char kThermal[] = "thermal";
static const char kGrabCut[] = "grabcut";
static const char kColorHistogram[] = "colorhist";
static const char kFloodFill[] = "floodfill";
static const char kBox[] = "box";

class Labeling {
 public:
  // Takes in an open bag to write to. If bag is NULL, then does not write to
  // bag. If output_dir is "", does not save images to the output dir. If so, it
  // writes images to 00000-color.png, 00000-depth.png, 00000-labels.png, etc.
  // The counter is reset with each instance of Labeling.
  Labeling(const Projection& projection, Nerf* nerf,
           const std::string& output_dir, rosbag::Bag* output_bag);

  void Process(const sensor_msgs::Image::ConstPtr& rgb,
               const sensor_msgs::Image::ConstPtr& depth,
               const sensor_msgs::Image::ConstPtr& thermal);
  // If debug is true, then wait for a keypress between each image.
  void set_debug(bool debug);
  void set_labeling_algorithm(const std::string& alg);

 private:
  void LabelWithThermal(cv::Mat thermal_projected, cv::Mat near_hand_mask,
                        int rows, int cols, float thermal_threshold,
                        cv::OutputArray labels);
  void LabelWithGrabCut(const sensor_msgs::ImageConstPtr& rgb, int rows,
                        int cols, cv::Mat thermal_projected,
                        cv::Mat near_hand_mask, float thermal_threshold,
                        cv::OutputArray labels);
  void LabelWithBox(float4* points, cv::Mat mask, int rows, int cols,
                    Eigen::Vector3f l_hand_pos, Eigen::Vector3f r_hand_pos,
                    bool debug, cv::OutputArray labels);

  Projection projection_;
  Nerf* nerf_;
  rosbag::Bag* output_bag_;
  std::string output_dir_;
  bool debug_;
  ros::NodeHandle nh_;
  ros::Publisher skeleton_pub_;
  ros::Publisher rgb_pub_;
  ros::Publisher depth_pub_;
  ros::Publisher depth_info_pub_;
  ros::Publisher thermal_pub_;
  ros::Publisher cloud_pub_;
  ros::Publisher overlay_pub_;
  std::string labeling_algorithm_;

  CameraData camera_data_;
  sensor_msgs::CameraInfo rgbd_info_;

  double thermal_threshold_;
  int frame_count_;
};

struct HandBoxCoords {
  float min_x;
  float max_x;
  float min_y;
  float max_y;
  float min_z;
  float max_z;
};

void ComputeHandMask(float4* points, int height, int width, const float min_x,
                     const float max_x, const float min_y, const float max_y,
                     const float min_z, const float max_z,
                     const CameraData& camera_data,
                     const Eigen::Affine3f& l_forearm_pose,
                     const Eigen::Affine3f& r_forearm_pose, uint8_t* mask);

void MaskToIndices(uint8_t* mask, int len,
                   pcl::PointIndices::Ptr near_hand_indices);
// Converts a list of indices in an ordered point cloud to a 2D mask.
// This "appends" to the mask, so you can call this multiple times with the same
// mask.
void IndicesToMask(const std::vector<int>& indices, int rows, int cols,
                   cv::OutputArray mask);

void GetPointCloud(const float4* points, const sensor_msgs::Image& rgb,
                   pcl::PointCloud<pcl::PointXYZRGB>::Ptr point_cloud);

void HandBoxMarkers(const HandBoxCoords& hand_box,
                    const Eigen::Affine3f l_matrix,
                    const Eigen::Affine3f r_matrix,
                    const Eigen::Vector3f l_hand_pos,
                    const Eigen::Vector3f r_hand_pos,
                    visualization_msgs::MarkerArray* boxes);

void LabelWithReducedColorComponents(cv::Mat rgb, cv::Mat near_hand_mask,
                                     cv::Mat thermal_projected,
                                     double thermal_threshold,
                                     cv::OutputArray labels);

void LabelWithFloodFill(cv::Mat rgb, cv::Mat near_hand_mask,
                        cv::Mat thermal_projected, double thermal_threshold,
                        bool debug, cv::OutputArray labels);

// Reduces the color space such that there are only num_bins of R, G, and B.
// Also uses the mask to only process points near the hand.
void ReduceRgb(cv::Mat rgb, cv::Mat near_hand_mask, int num_bins,
               cv::OutputArray reduced);

// Given an RGB image (should be reduced to a small number of colors), finds the
// connected components, where two adjacent pixels are connected if they have
// the same color. Returns a list of clusters, where each cluster is a list of
// points.
void FindConnectedComponents(cv::Mat reduced_rgb, cv::Mat near_hand_mask,
                             std::vector<std::vector<cv::Point> >* clusters);

}  // namespace skinseg

#endif  // _SKINSEG_LABELING_H_
