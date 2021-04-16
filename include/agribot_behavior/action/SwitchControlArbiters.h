/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <behaviortree_cpp_v3/action_node.h>
#include <string>

namespace agribot_behavior
{
class SwitchControlArbiters : public BT::SyncActionNode
{
public:
  SwitchControlArbiters(const std::string& name, const BT::NodeConfiguration& config);
  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;
};
}  // namespace agribot_behavior
