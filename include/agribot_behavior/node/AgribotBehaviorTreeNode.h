/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <behaviortree_cpp_v3/bt_factory.h>
#include <behaviortree_cpp_v3/loggers/bt_zmq_publisher.h>
#include <agribot_behavior/action/CheckStartConditions.h>
#include <agribot_behavior/action/SwitchControlArbiters.h>
#include <agribot_behavior/action/Handle.h>

#include <ros/ros.h>
#include <ros/package.h>

#include <string>
#include <memory>


namespace agribot_behavior
{
class AgribotBehaviorTreeNode
{
private:
  std::unique_ptr<BT::BehaviorTreeFactory> factory_;
  std::unique_ptr<BT::Tree> tree_;

  std::shared_ptr<ros::NodeHandle> node_handle_;
  std::unique_ptr<BT::PublisherZMQ> tree_state_publisher_; //used by Groot

public:
  void registerAllNodes();
  std::string getTreeLocation() const;
  AgribotBehaviorTreeNode(int argc, char** argv);
  void run();
};
}  // namespace agribot_behavior
