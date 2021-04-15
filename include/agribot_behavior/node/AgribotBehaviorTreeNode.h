/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <cstdlib>
#include <behaviortree_cpp_v3/bt_factory.h>
#include <agribot_behavior/action/CheckStartConditions.h>
#include <agribot_behavior/action/SwitchControlArbiters.h>
#include <agribot_behavior/action/Handle.h>

#include <ros/package.h>

namespace agribot_behavior
{
class AgribotBehaviorTreeNode
{
private:
  BT::BehaviorTreeFactory factory;
  BT::Tree tree;

public:
  void registerAllNodes();
  std::string getTreeLocation() const;
  AgribotBehaviorTreeNode();
  void run();
};
}  // namespace agribot_behavior
