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
  bool fileExists(const std::string& name);
  void setPose(const YAML::Node& node, std::size_t index,
               std::vector<agribot_msgs::NavSatFixOrientation>& path_points);  // NOLINT

public:
  bool setPathName(const std::string& name);
  std::vector<agribot_msgs::NavSatFixOrientation> parsePath();
};
}  // namespace agribot_behavior
