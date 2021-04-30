/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_msgs/AgribotMoveBaseMonitorResult.h>
#include <industrial_msgs/RobotMode.h>

#include <shared_mutex>
#include <string>

namespace agribot_behavior
{
// Thread safety is guaranteed by SharedField class and mutex lock mechanism
template <typename T>
class SharedField
{
private:
  std::shared_timed_mutex mutex;
  T data;

public:
  void writeData(const T& data_to_write);
  T getData();
};
}  // namespace agribot_behavior
