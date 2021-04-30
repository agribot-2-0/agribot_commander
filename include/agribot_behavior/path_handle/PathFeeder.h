/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_behavior/path_handle/PathParser.h>
#include <agribot_msgs/NavSatFixOrientation.h>

#include <string>
#include <vector>
#include <memory>

namespace agribot_behavior
{
class PathFeeder
{
private:
  std::unique_ptr<PathParser> path_parser_;
  std::size_t path_index_;
  bool feeding_completed_;
  std::vector<agribot_msgs::NavSatFixOrientation> path_points_;

public:
  void init(const std::string& path_folder, const std::string& task_folder);
  bool loadPath(const std::string& path_name);
  agribot_msgs::NavSatFixOrientation getPathPoint();
  void reset();
  bool feedingCompleted();
  std::string getLoadedPathLocation();
};
}  // namespace agribot_behavior
