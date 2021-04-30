/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_msgs/NavSatFixOrientation.h>

#include <yaml-cpp/yaml.h>

#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>

namespace agribot_behavior
{
class PathParser
{
private:
  std::string path_location_;
  std::string path_folder_;
  std::string task_folder_;

  std::string getPathNameFromTask(const std::string& task_name);
  bool fileExists(const std::string& name);
  void setPose(const YAML::Node& node, std::size_t index,
               std::vector<agribot_msgs::NavSatFixOrientation>& path_points);  // NOLINT

public:
  PathParser() = delete;
  PathParser(const std::string& path_folder, const std::string& task_folder);
  bool setPathName(const std::string& task_name);
  std::vector<agribot_msgs::NavSatFixOrientation> parsePath();
  std::string getPathLocation() const;
};
}  // namespace agribot_behavior
