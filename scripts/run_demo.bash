#! /bin/bash

source /home/jstn/.bashrc
source /opt/ros/indigo/setup.bash
source /home/jstn/tracking_ws/devel/setup.sh
#rosrun skin_segmentation hand_segmentation_demo.py /home/jstn/data/hand_models/2017-11-16_data24_100k/vgg16_fcn_rgbd_single_frame_hand_iter_100000.ckpt rgb:=/camera/rgb/image_rect_color depth_registered:=/camera/depth_registered/image
#rosrun skin_segmentation hand_segmentation_demo.py /home/jstn/data/hand_models/2017-11-17_data24_chromatic_100k/vgg16_fcn_rgbd_single_frame_hand_iter_100000.ckpt rgb:=/camera/rgb/image_rect_color depth_registered:=/camera/depth_registered/image
#rosrun skin_segmentation hand_segmentation_demo.py /home/jstn/data/hand_models/2017-11-18_data24_h25_100k/vgg16_fcn_rgbd_single_frame_hand_iter_100000.ckpt rgb:=/camera/rgb/image_rect_color depth_registered:=/camera/depth_registered/image
rosrun skin_segmentation hand_segmentation_demo.py /home/jstn/data/hand_models/2017-11-21_data25_h20_100k/vgg16_fcn_rgbd_single_frame_hand_iter_100000.ckpt rgb:=/camera/rgb/image_rect_color depth_registered:=/camera/depth_registered/image
