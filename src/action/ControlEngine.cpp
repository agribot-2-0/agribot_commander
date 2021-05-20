/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/action/ControlEngine.h>

namespace agribot_behavior
{
ControlEngine::ControlEngine(const std::string& name, const BT::NodeConfiguration& config)
  : SyncActionNode(name, config)
{
}

BT::PortsList ControlEngine::providedPorts()
{
  return { BT::InputPort<agribot_behavior::ControlEngineAction>("action") };
}

BT::NodeStatus ControlEngine::tick()
{
  BT::Optional<agribot_behavior::ControlEngineAction> msg =
      TreeNode::getInput<agribot_behavior::ControlEngineAction>("action");
  if (!msg)
  {
    ROS_ERROR_STREAM("[BehaviorTree][ControlEngine] Missing required input [action]: " << msg.error());
    return BT::NodeStatus::FAILURE;
  }
  else
  {
    switch (msg.value())
    {
      case agribot_behavior::ControlEngineAction::Start:
        return startEngine();
      case agribot_behavior::ControlEngineAction::Stop:
        return stopEngine();
      default:
        return BT::NodeStatus::FAILURE;
    }
  }
  return BT::NodeStatus::FAILURE;
}

void ControlEngine::initEngineControlClients(std::shared_ptr<ros::ServiceClient> start_engine_client,
                                             std::shared_ptr<ros::ServiceClient> stop_engine_client)
{
  start_engine_client_ = start_engine_client;
  stop_engine_client_ = stop_engine_client;
}

BT::NodeStatus ControlEngine::startEngine()
{
  std_srvs::Trigger trigger;
  if (!start_engine_client_->call(trigger))
  {
    ROS_ERROR_STREAM("[BehaviorTree][ControlEngine] Failed to start engine. " << trigger.response);
    return BT::NodeStatus::FAILURE;
  }
  ROS_WARN_STREAM("[BehaviorTree][ControlEngine] Engine started");
  return BT::NodeStatus::SUCCESS;
}

BT::NodeStatus ControlEngine::stopEngine()
{
  std_srvs::Trigger trigger;
  if (!stop_engine_client_->call(trigger))
  {
    ROS_ERROR_STREAM("[BehaviorTree][ControlEngine] Failed to stop engine. " << trigger.response);
    return BT::NodeStatus::FAILURE;
  }
  ROS_WARN_STREAM("[BehaviorTree][ControlEngine] Engine stopped");
  return BT::NodeStatus::SUCCESS;
}

}  // namespace agribot_behavior
