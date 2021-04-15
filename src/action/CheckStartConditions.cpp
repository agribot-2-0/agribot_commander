/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/action/CheckStartConditions.h>

namespace agribot_behavior
{
CheckStartConditions::CheckStartConditions(const std::string& name) : BT::SyncActionNode(name, {})
{
}

BT::NodeStatus CheckStartConditions::tick()
{
  return BT::NodeStatus::SUCCESS;
}
}  // namespace agribot_behavior
