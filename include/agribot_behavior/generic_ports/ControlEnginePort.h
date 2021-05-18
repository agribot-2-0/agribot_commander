/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_common/common_enums/ControlEngineAction.h>
#include <behaviortree_cpp_v3/basic_types.h>

namespace BT
{
template <>
inline agribot_behavior::ControlEngineAction convertFromString(StringView str)
{
  if (str == "start")
  {
    return agribot_behavior::ControlEngineAction::Start;
  }
  else if (str == "stop")
  {
    return agribot_behavior::ControlEngineAction::Stop;
  }
  else
  {
    throw RuntimeError("Invalid input from convertFromString for ControlEngineAction");
  }
}
}  // end namespace BT
