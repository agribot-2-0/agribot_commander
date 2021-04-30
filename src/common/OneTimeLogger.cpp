/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/common/OneTimeLogger.h>

namespace agribot_behavior
{
void OneTimeLogger::log(std::string_view name, ros::console::Level level, const std::string& message)
{
  ROS_LOG_STREAM_COND(!logged_once_[name].load(), level, ROSCONSOLE_DEFAULT_NAME, message);
  logged_once_[name].store(true);
}
void OneTimeLogger::reset(std::string_view name)
{
  logged_once_[name].store(false);
}
}  // namespace agribot_behavior
