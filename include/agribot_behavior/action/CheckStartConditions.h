/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <behaviortree_cpp_v3/action_node.h>
#include <agribot_behavior/generic_ports/ControlSourcePort.h>

#include <agribot_behavior/common/SharedField.h>
#include <agribot_common/common_enums/ControlSource.h>

#include <ros/console.h>
#include <industrial_msgs/RobotMode.h>

#include <memory>
#include <string>

namespace agribot_behavior
{
class CheckStartConditions : public BT::SyncActionNode
{
private:
  std::shared_ptr<SharedField<std::string>> current_task_name_;
  std::shared_ptr<SharedField<industrial_msgs::RobotMode>> robot_mode_;

  BT::NodeStatus checkManualCondition();
  BT::NodeStatus checkNavigationCondition();

public:
  CheckStartConditions(const std::string& name, const BT::NodeConfiguration& config);
  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;
  void initSharedFields(std::shared_ptr<SharedField<std::string>> current_task_name,
                        std::shared_ptr<SharedField<industrial_msgs::RobotMode>> robot_mode);
};
}  // namespace agribot_behavior
