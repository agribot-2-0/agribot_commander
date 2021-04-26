/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_behavior/path_handle/PathParser.h>
#include <agribot_msgs/NavSatFixOrientation.h>

#include <string>
#include <vector>

namespace agribot_behavior
{
class PathFeeder
{
private:
  PathParser path_parser_;
  std::size_t path_index_;
  std::vector<agribot_msgs::NavSatFixOrientation> path_points_;

public:
  bool loadPath(const std::string& path_name);
  agribot_msgs::NavSatFixOrientation feed();
  void reset();
};
}  // namespace agribot_behavior
