/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <behaviortree_cpp_v3/bt_factory.h>
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
  BT::BehaviorTreeFactory factory;
  BT::Tree tree;

  std::shared_ptr<ros::NodeHandle> node_handle_;

public:
  void registerAllNodes();
  std::string getTreeLocation() const;
  AgribotBehaviorTreeNode(int argc, char** argv);
  void run();
};
}  // namespace agribot_behavior
