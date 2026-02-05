#pragma once

#include <Eigen/Geometry>
#include <apriltag/common/matd.h>
#include <apriltag_pose.h>
#include <geometry_msgs/msg/quaternion.hpp>
#include <geometry_msgs/msg/vector3.hpp>
#include <geometry_msgs/msg/transform.hpp>
#include <opencv2/core/mat.hpp>

namespace apriltag_ros {

void convertQuaternion(const Eigen::Quaterniond& eigen_quat, geometry_msgs::msg::Quaternion& msg_quat);
void convertVector3(const matd_t& mat, geometry_msgs::msg::Vector3& msg_vec);
void convertVector3(const cv::Mat_<double>& vec, geometry_msgs::msg::Vector3& msg_vec);
geometry_msgs::msg::Transform poseToTransform(const apriltag_pose_t& pose);
geometry_msgs::msg::Transform poseToTransform(const std::pair<cv::Mat_<double>, cv::Mat_<double>>& pose);

} // namespace apriltag_ros

// For backward compatibility with tf2 namespace
// Note: In Foxy, we use regular overloaded functions, not template specializations
namespace tf2 {

void convert(const Eigen::Quaterniond& eigen_quat, geometry_msgs::msg::Quaternion& msg_quat);
void convert(const matd_t& mat, geometry_msgs::msg::Vector3& msg_vec);
void convert(const cv::Mat_<double>& vec, geometry_msgs::msg::Vector3& msg_vec);

geometry_msgs::msg::Transform toMsg(const apriltag_pose_t& pose);
geometry_msgs::msg::Transform toMsg(const std::pair<cv::Mat_<double>, cv::Mat_<double>>& pose);

} // namespace tf2