/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_msgs/LampsControl.h>
#include <behaviortree_cpp_v3/basic_types.h>

namespace BT
{
template <>
inline agribot_msgs::LampsControl convertFromString(StringView str)
{
  agribot_msgs::LampsControl srv;
  srv.request.lamp = srv.request.left_back_lamp | srv.request.right_back_lamp | srv.request.left_front_lamp |
                     srv.request.right_front_lamp;
  srv.request.intensity = 100;
  if (str == "ready")
  {
    srv.request.color = srv.request.green;
    srv.request.flash_period_in_miliseconds = 1000;
    srv.request.fading_period_in_miliseconds = 1000;
    return srv;
  }
  else if (str == "auto")
  {
    srv.request.color = srv.request.green;
    srv.request.flash_period_in_miliseconds = 0;
    srv.request.fading_period_in_miliseconds = 0;
    return srv;
  }
  else if (str == "manual")
  {
    srv.request.color = srv.request.blue;
    srv.request.flash_period_in_miliseconds = 0;
    srv.request.fading_period_in_miliseconds = 0;
    return srv;
  }
  else if (str == "error")
  {
    srv.request.color = srv.request.red;
    srv.request.flash_period_in_miliseconds = 1000;
    srv.request.fading_period_in_miliseconds = 1000;
    return srv;
  }
  else if (str == "obstacle")
  {
    srv.request.color = srv.request.orange;
    srv.request.flash_period_in_miliseconds = 1000;
    srv.request.fading_period_in_miliseconds = 1000;
    return srv;
  }
  else
  {
    throw RuntimeError("Invalid input from convertFromString for LampsControl");
  }
}
}  // end namespace BT
