// Copyright 2025 Lihan Chen
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "pb2025_sentry_behavior/plugins/action/pub_joint_state.hpp"

namespace pb2025_sentry_behavior
{

PublishJointStateAbsolute::PublishJointStateAbsolute(
  const std::string & name, const BT::NodeConfig & config)
: RosTopicPubStatefulActionNode(name, config, BT::RosNodeParams())
{
}

BT::PortsList PublishJointStateAbsolute::providedPorts()
{
  return {
    BT::InputPort<float>("gimbal_pitch", "Expected Pitch angle (rad)"),
    BT::InputPort<float>("gimbal_yaw", "Expected Yaw angle (rad)"),
    BT::InputPort<std::chrono::milliseconds>("duration", "Publish duration"),
    BT::InputPort<std::string>("topic_name", "__default__placeholder__", "Topic name")};
}

bool PublishJointStateAbsolute::setMessage(pb_rm_interfaces::msg::GimbalCmd & msg)
{
  msg.header.stamp = node_->now();
  msg.header.frame_id = "gimbal";
  msg.yaw_type = pb_rm_interfaces::msg::GimbalCmd::ABSOLUTE_ANGLE;
  msg.pitch_type = pb_rm_interfaces::msg::GimbalCmd::ABSOLUTE_ANGLE;

  float pitch, yaw;
  if (!getInput("gimbal_pitch", pitch) || !getInput("gimbal_yaw", yaw)) {
    RCLCPP_ERROR(node_->get_logger(), "Missing angle parameters");
    return false;
  }

  msg.position.pitch = pitch;
  msg.position.yaw = yaw;
  msg.velocity = pb_rm_interfaces::msg::Gimbal();

  return true;
}

PublishJointStateVelocity::PublishJointStateVelocity(
  const std::string & name, const BT::NodeConfig & config)
: RosTopicPubStatefulActionNode(name, config, BT::RosNodeParams())
{
}

BT::PortsList PublishJointStateVelocity::providedPorts()
{
  return {
    BT::InputPort<float>("gimbal_vel_pitch", 0.0f, "Pitch velocity (rad/s)"),
    BT::InputPort<float>("gimbal_vel_yaw", 0.0f, "Yaw velocity (rad/s)"),
    BT::InputPort<float>("pitch_min", -1.57f, "Minimum pitch range"),
    BT::InputPort<float>("pitch_max", 1.57f, "Maximum pitch range"),
    BT::InputPort<float>("yaw_min", -3.14f, "Minimum yaw range"),
    BT::InputPort<float>("yaw_max", 3.14f, "Maximum yaw range"),
    BT::InputPort<std::chrono::milliseconds>("duration", "Publish duration"),
    BT::InputPort<std::string>("topic_name", "__default__placeholder__", "Topic name")};
}

bool PublishJointStateVelocity::setMessage(pb_rm_interfaces::msg::GimbalCmd & msg)
{
  msg.header.stamp = node_->now();
  msg.header.frame_id = "gimbal";
  msg.yaw_type = pb_rm_interfaces::msg::GimbalCmd::VELOCITY;
  msg.pitch_type = pb_rm_interfaces::msg::GimbalCmd::VELOCITY;

  float vel_pitch, vel_yaw;
  if (!getInput("gimbal_vel_pitch", vel_pitch) || !getInput("gimbal_vel_yaw", vel_yaw)) {
    RCLCPP_ERROR(node_->get_logger(), "Missing velocity parameters");
    return false;
  }

  float pitch_min, pitch_max, yaw_min, yaw_max;
  if (
    !getInput("pitch_min", pitch_min) || !getInput("pitch_max", pitch_max) ||
    !getInput("yaw_min", yaw_min) || !getInput("yaw_max", yaw_max)) {
    RCLCPP_ERROR(node_->get_logger(), "Missing range parameters");
    return false;
  }

  if (pitch_min >= pitch_max || yaw_min >= yaw_max) {
    RCLCPP_ERROR(
      node_->get_logger(), "Invalid range values: [%.2f, %.2f] [%.2f, %.2f]", pitch_min, pitch_max,
      yaw_min, yaw_max);
    return false;
  }

  msg.velocity.pitch = vel_pitch;
  msg.velocity.yaw = vel_yaw;
  msg.velocity.pitch_min_range = pitch_min;
  msg.velocity.pitch_max_range = pitch_max;
  msg.velocity.yaw_min_range = yaw_min;
  msg.velocity.yaw_max_range = yaw_max;

  msg.position = pb_rm_interfaces::msg::GimbalPosition();

  return true;
}

}  // namespace pb2025_sentry_behavior

#include "behaviortree_ros2/plugins.hpp"
CreateRosNodePlugin(pb2025_sentry_behavior::PublishJointStateAbsolute, "PublishJointStateAbsolute");
CreateRosNodePlugin(pb2025_sentry_behavior::PublishJointStateVelocity, "PublishJointStateVelocity");

// #include <pluginlib/class_list_macros.hpp>
// // 使用标准宏注册插件
// PLUGINLIB_EXPORT_CLASS(
//   pb2025_sentry_behavior::PublishJointStateAbsolute,
//   bt::RosActionNode
// );
// PLUGINLIB_EXPORT_CLASS(
//   pb2025_sentry_behavior::PublishJointStateVelocity,
//   bt::RosActionNode
// );
