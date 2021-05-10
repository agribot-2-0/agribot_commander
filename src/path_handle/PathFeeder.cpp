/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/path_handle/PathFeeder.h>

namespace agribot_behavior
{
void PathFeeder::init(const std::experimental::filesystem::path& path_folder,
                      const std::experimental::filesystem::path& task_folder)
{
  path_parser_ = std::make_unique<PathParser>(path_folder, task_folder);
}

bool PathFeeder::loadPath(const std::experimental::filesystem::path& task_name)
{
  reset();
  if (path_parser_->setPathName(task_name))
  {
    path_points_ = path_parser_->parsePath();
    return true;
  }
  return false;
}

sensor_msgs::NavSatFix PathFeeder::getPathPoint()
{
  if (path_index_ < path_points_.size())
  {
    sensor_msgs::NavSatFix pose = path_points_[path_index_];
    ++path_index_;
    return pose;
  }
  else
  {
    feeding_completed_ = true;
    return path_points_[--path_index_];  // when all the points from path are send, send the last point again
  }
}

void PathFeeder::reset()
{
  feeding_completed_ = false;
  path_index_ = 0;
}

bool PathFeeder::feedingCompleted()
{
  if (feeding_completed_)
  {
    reset();
    return true;
  }
  return feeding_completed_;
}

std::experimental::filesystem::path PathFeeder::getLoadedPathLocation()
{
  return path_parser_->getPathLocation();
}

}  // namespace agribot_behavior
