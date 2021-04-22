/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <behaviortree_cpp_v3/action_node.h>

#include <agribot_common/common_enums/ControlSource.h>
#include <ros/console.h>

#include <atomic>
#include <string>
#include <chrono>  // NOLINT
// todo(mwiadoms): remove chrono header

namespace agribot_behavior
{
class Handle : public BT::AsyncActionNode
{
private:
  std::atomic<bool> halt_requested_;

public:
  Handle(const std::string& name, const BT::NodeConfiguration& config);
  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;
  void halt() override;
  void handleManualControl();
  void handleAutonomousControl();
};
}  // namespace agribot_behavior
