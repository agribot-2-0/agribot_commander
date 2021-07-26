/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_behavior/common/SharedField.h>
#include <agribot_behavior/generic_ports/StreamingControlPort.h>
#include <behaviortree_cpp_v3/action_node.h>
#include <ros/console.h>
#include <ros/ros.h>
#include <std_srvs/Empty.h>

#include <memory>
#include <string>

namespace agribot_behavior
{
class StreamingControl : public BT::SyncActionNode
{
private:
  std::shared_ptr<ros::ServiceClient> hal_video_stream_start_client_;
  std::shared_ptr<ros::ServiceClient> hal_video_stream_stop_client_;

public:
  StreamingControl(const std::string& name, const BT::NodeConfiguration& config);
  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;
  void initVideoStreamClients(std::shared_ptr<ros::ServiceClient> hal_video_stream_start_client,
                              std::shared_ptr<ros::ServiceClient> hal_video_stream_stop_client);
};
}  // namespace agribot_behavior
