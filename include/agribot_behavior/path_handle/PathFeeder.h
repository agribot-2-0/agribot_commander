/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_behavior/path_handle/PathParser.h>
#include <sensor_msgs/NavSatFix.h>

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
  std::vector<sensor_msgs::NavSatFix> path_points_;

public:
  void init(const std::experimental::filesystem::path& path_folder,
            const std::experimental::filesystem::path& task_folder);
  bool loadPath(const std::experimental::filesystem::path& path_name);
  sensor_msgs::NavSatFix getPathPoint();
  void reset();
  bool feedingCompleted();
  std::experimental::filesystem::path getLoadedPathLocation();
};
}  // namespace agribot_behavior
