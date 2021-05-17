/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_common/common_enums/TtcontrolState.h>
#include <behaviortree_cpp_v3/basic_types.h>

namespace BT
{
template <>
inline agribot_behavior::TtcontrolState convertFromString(StringView str)
{
  if (str == "safe_state")
  {
    return agribot_behavior::TtcontrolState::SafeState;
  }
  else if (str == "auto")
  {
    return agribot_behavior::TtcontrolState::Autonomous;
  }
  else
  {
    throw RuntimeError("Invalid input from convertFromString for TtcontrolState");
  }
}
}  // end namespace BT
