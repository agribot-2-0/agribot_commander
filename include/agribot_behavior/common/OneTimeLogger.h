/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <ros/console.h>
#include <sstream>
#include <unordered_map>
#include <string>
#include <string_view>
#include <atomic>

namespace agribot_behavior
{
class OneTimeLogger
{
private:
  std::unordered_map<std::string_view, std::atomic<bool>> logged_once_;

public:
  void log(std::string_view name, ros::console::Level level, const std::string& message);
  void reset(std::string_view name);
};

}  // namespace agribot_behavior
