/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_common/common_enums/ControlSource.h>
#include <behaviortree_cpp_v3/basic_types.h>

namespace BT
{
template <>
inline tool_manager::ControlSource convertFromString(StringView str)
{
  if (str == "manual")
  {
    return tool_manager::ControlSource::Manual;
  }
  else if (str == "navigation")
  {
    return tool_manager::ControlSource::Navigation;
  }
  else if (str == "none")
  {
    return tool_manager::ControlSource::None;
  }
  else
  {
    throw RuntimeError("Invalid input from convertFromString for ControlSource");
  }
}
}  // end namespace BT
