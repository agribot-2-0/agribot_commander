/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/path_handle/PathFeeder.h>

namespace agribot_behavior
{
bool PathFeeder::loadPath(const std::string& path_name)
{
  if (path_parser_.setPathName(path_name))
  {
    path_points_ = path_parser_.parsePath();
    return true;
  }
  return false;
}

agribot_msgs::NavSatFixOrientation PathFeeder::feed()
{
  if (path_index_ < path_points_.size())
  {
    agribot_msgs::NavSatFixOrientation pose = path_points_[path_index_];
    ++path_index_;
    return pose;
  }
  else if (path_index_ == path_points_.size())
  {
    --path_index_;
    return path_points_[path_index_];
  }
}

void PathFeeder::reset()
{
  path_index_ = 0;
}

}  // namespace agribot_behavior
