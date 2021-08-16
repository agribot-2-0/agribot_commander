/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <sensor_msgs/NavSatFix.h>

#include <yaml-cpp/yaml.h>
#include <agribot_msgs/AgribotTool.h>
#include <agribot_behavior/common/WeatherConditions.h>
#include <ros/ros.h>

#include <experimental/filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>

namespace agribot_behavior
{
enum class TplLocation
{
  front,
  back
};

class TaskParser
{
private:
  std::experimental::filesystem::path path_location_;
  std::experimental::filesystem::path path_folder_;
  std::experimental::filesystem::path task_folder_;
  std::experimental::filesystem::path task_location_;

  agribot_msgs::AgribotTool tpl_front_config_;
  agribot_msgs::AgribotTool tpl_back_config_;
  WeatherConditions weather_conditions_from_task_;

  std::experimental::filesystem::path getPathNameFromTask();
  bool fileExists(const std::experimental::filesystem::path& name);
  void setPose(const YAML::Node& node, std::size_t index,
               std::vector<sensor_msgs::NavSatFix>& path_points);  // NOLINT
  void setTaskLocation(const std::experimental::filesystem::path& task_name);
  bool setPathName();
  void parseTpl(TplLocation tpl);
  void parseWeatherConditions();
  std::string convertTplLocationToString(TplLocation tpl);
  Condition convertWeatherConditionToEnumCondition(const std::string& condition);

public:
  TaskParser() = delete;
  TaskParser(const std::experimental::filesystem::path& path_folder,
             const std::experimental::filesystem::path& task_folder);

  bool setPathName(const std::experimental::filesystem::path& task_name);
  std::vector<sensor_msgs::NavSatFix> parsePath();
  std::experimental::filesystem::path getPathLocation() const;
  agribot_msgs::AgribotTool getTplFrontConfig();
  agribot_msgs::AgribotTool getTplBackConfig();
  WeatherConditions getWeatherConditions();
  void parseTask(const std::experimental::filesystem::path& task_name);
};
}  // namespace agribot_behavior
