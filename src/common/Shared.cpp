/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/common/Shared.h>

namespace agribot_behavior
{
template <typename T>
void Shared<T>::writeData(const T& data_to_write)
{
  std::lock_guard<std::shared_timed_mutex> writerLock(mutex);
  data = data_to_write;
}

template <typename T>
T Shared<T>::getData()
{
  std::shared_lock<std::shared_timed_mutex> readerLock(mutex);
  return data;
}


template void Shared<std::string>::writeData(const std::string&);
template std::string Shared<std::string>::getData();
template void Shared<industrial_msgs::RobotMode>::writeData(const industrial_msgs::RobotMode&);
template industrial_msgs::RobotMode Shared<industrial_msgs::RobotMode>::getData();
}  // namespace agribot_behavior
