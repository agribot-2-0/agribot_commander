/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <sensor_msgs/NavSatFix.h>

#include <yaml-cpp/yaml.h>
#include <agribot_msgs/AgribotTool.h>

#include <experimental/filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>

namespace agribot_behavior
{
enum whichTpl
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

  agribot_msgs::AgribotTool tpl_front_config_;
  agribot_msgs::AgribotTool tpl_back_config_;

  std::experimental::filesystem::path getPathNameFromTask(const std::experimental::filesystem::path& task_name);
  bool fileExists(const std::experimental::filesystem::path& name);
  void setPose(const YAML::Node& node, std::size_t index,
               std::vector<sensor_msgs::NavSatFix>& path_points);  // NOLINT

public:
  TaskParser() = delete;
  TaskParser(const std::experimental::filesystem::path& path_folder,
             const std::experimental::filesystem::path& task_folder);
  bool setPathName(const std::experimental::filesystem::path& task_name);
  std::vector<sensor_msgs::NavSatFix> parsePath();
  std::experimental::filesystem::path getPathLocation() const;
  void parseTpl(const std::experimental::filesystem::path& task_name, whichTpl tpl);
  agribot_msgs::AgribotTool getTplFrontConfig();
  agribot_msgs::AgribotTool getTplBackConfig();
};
}  // namespace agribot_behavior
