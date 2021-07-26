/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/action/StreamingControl.h>

namespace agribot_behavior
{
StreamingControl::StreamingControl(const std::string& name, const BT::NodeConfiguration& config)
  : SyncActionNode(name, config)
{
}

BT::PortsList StreamingControl::providedPorts()
{
  return { BT::InputPort<bool>("streaming_control") };
}

BT::NodeStatus StreamingControl::tick()
{
  std_srvs::Empty srv;
  BT::Optional<bool> msg = TreeNode::getInput<bool>("streaming_control");

  if (!msg)
  {
    ROS_ERROR_STREAM("[BehaviorTree][StreamingControl] Missing required input [streaming_control]: " << msg.error());
  }
  else if (msg.value())
  {
    if (hal_video_stream_start_client_->call(srv))
    {
      return BT::NodeStatus::SUCCESS;
    }
  }
  else
  {
    if (hal_video_stream_stop_client_->call(srv))
    {
      return BT::NodeStatus::SUCCESS;
    }
  }

  return BT::NodeStatus::FAILURE;
}

void StreamingControl::initVideoStreamClients(std::shared_ptr<ros::ServiceClient> hal_video_stream_start_client,
                                              std::shared_ptr<ros::ServiceClient> hal_video_stream_stop_client)
{
  hal_video_stream_start_client_ = hal_video_stream_start_client;
  hal_video_stream_stop_client_ = hal_video_stream_stop_client;
}

// bool StreamingControl::controlVideoStream(const tool_manager::ControlSource selected_source)
// {
//   std_srvs::Empty srv;
//   bool result = false;
//   switch (selected_source)
//   {
//     case tool_manager::ControlSource::Manual:
//       result = hal_video_stream_start_client_->call(srv);
//       break;
//     case tool_manager::ControlSource::Navigation:
//       result = hal_video_stream_stop_client_->call(srv);
//       break;
//     case tool_manager::ControlSource::None:
//       result = hal_video_stream_stop_client_->call(srv);
//       break;
//     default:
//       break;
//   }
//   return result;
// }

}  // namespace agribot_behavior