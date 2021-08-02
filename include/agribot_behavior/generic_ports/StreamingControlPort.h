/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <behaviortree_cpp_v3/basic_types.h>
#include <ros/ros.h>

namespace BT
{
template <>
inline bool convertFromString(StringView str)
{
  if (str == "on")
  {
    return true;
  }
  else if (str == "off")
  {
    return false;
  }
  else
  {
    throw RuntimeError("Invalid input from convertFromString for StreamingControl");
  }
}
}  // end namespace BT
