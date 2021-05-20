/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/common/SharedField.h>

namespace agribot_behavior
{
template <typename T>
void SharedField<T>::writeData(const T& data_to_write)
{
  std::lock_guard<std::shared_timed_mutex> writerLock(mutex);
  data = data_to_write;
}

template <typename T>
T SharedField<T>::getData()
{
  std::shared_lock<std::shared_timed_mutex> readerLock(mutex);
  return data;
}

template void SharedField<std::string>::writeData(const std::string&);
template std::string SharedField<std::string>::getData();
template void SharedField<agribot_msgs::AgribotNavigationManagerResult>::writeData(
    const agribot_msgs::AgribotNavigationManagerResult&);
template agribot_msgs::AgribotNavigationManagerResult
SharedField<agribot_msgs::AgribotNavigationManagerResult>::getData();
template void SharedField<std::size_t>::writeData(const std::size_t&);
template std::size_t SharedField<std::size_t>::getData();
template void SharedField<agribot_msgs::AgribotTtcontrolState>::writeData(const agribot_msgs::AgribotTtcontrolState&);
template agribot_msgs::AgribotTtcontrolState SharedField<agribot_msgs::AgribotTtcontrolState>::getData();
template void SharedField<industrial_msgs::RobotMode>::writeData(const industrial_msgs::RobotMode&);
template industrial_msgs::RobotMode SharedField<industrial_msgs::RobotMode>::getData();
}  // namespace agribot_behavior
