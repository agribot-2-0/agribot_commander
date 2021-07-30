/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <yaml-cpp/yaml.h>
#include <ros/ros.h>
#include <agribot_msgs/AgribotTool.h>

#include <experimental/filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>

namespace agribot_behavior
{
class TplHandler
{
private:
  std::experimental::filesystem::path tpl_front_location_;
  std::experimental::filesystem::path tpl_back_location_;
  std::experimental::filesystem::path tpl_folder_;
  std::experimental::filesystem::path task_folder_;

  agribot_msgs::AgribotTool tpl_front_config_;
  agribot_msgs::AgribotTool tpl_back_config_;

  bool fileExists(const std::experimental::filesystem::path& name);

public:
  TplHandler() = delete;
  TplHandler(const std::experimental::filesystem::path& path_folder,
             const std::experimental::filesystem::path& task_folder);
  std::experimental::filesystem::path getPathLocation() const;
  void configureTpl(const std::experimental::filesystem::path& task_name);
  void publishTplConfig(std::shared_ptr<ros::Publisher> tpl_front_config_publisher,
                        std::shared_ptr<ros::Publisher> tpl_back_config_publisher);
};
}  // namespace agribot_behavior