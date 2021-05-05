/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_msgs/NavSatFixOrientation.h>

#include <yaml-cpp/yaml.h>

#include <experimental/filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>

namespace agribot_behavior
{
class PathParser
{
private:
  std::experimental::filesystem::path path_location_;
  std::experimental::filesystem::path path_folder_;
  std::experimental::filesystem::path task_folder_;

  std::experimental::filesystem::path getPathNameFromTask(const std::experimental::filesystem::path& task_name);
  bool fileExists(const std::experimental::filesystem::path& name);
  void setPose(const YAML::Node& node, std::size_t index,
               std::vector<agribot_msgs::NavSatFixOrientation>& path_points);  // NOLINT

public:
  PathParser() = delete;
  PathParser(const std::experimental::filesystem::path& path_folder, const std::experimental::filesystem::path& task_folder);
  bool setPathName(const std::experimental::filesystem::path& task_name);
  std::vector<agribot_msgs::NavSatFixOrientation> parsePath();
  std::experimental::filesystem::path getPathLocation() const;
};
}  // namespace agribot_behavior
