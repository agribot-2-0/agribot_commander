/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include "agribot_behavior/path_handle/TplHandler.h"

namespace agribot_behavior
{
TplHandler::TplHandler(const std::experimental::filesystem::path& tpl_folder,
                       const std::experimental::filesystem::path& task_folder)
{
  tpl_folder_ = tpl_folder;
  task_folder_ = task_folder;
}

bool TplHandler::fileExists(const std::experimental::filesystem::path& name)
{
  std::ifstream f(name.c_str());
  return f.good();
}

void TplHandler::configureTpl(const std::experimental::filesystem::path& task_name)
{
  std::experimental::filesystem::path task_location = task_folder_ / task_name;
  task_location += ".yaml";
  if (fileExists(task_location))
  {
    YAML::Node task_yaml = YAML::LoadFile(task_location);

    if (task_yaml["task"]["tpl_front"]["tpl_angle"])
    {
      tpl_front_config_.tpl_angle = task_yaml["task"]["tpl_front"]["tpl_angle"].as<double>();
    }
    if (task_yaml["task"]["tpl_front"]["tpl_height"])
    {
      tpl_front_config_.tpl_height = task_yaml["task"]["tpl_front"]["tpl_height"].as<double>();
    }
    if (task_yaml["task"]["tpl_front"]["red_valve_flow"])
    {
      tpl_front_config_.red_valve_flow = task_yaml["task"]["tpl_front"]["red_valve_flow"].as<double>();
    }
    if (task_yaml["task"]["tpl_front"]["green_valve_flow"])
    {
      tpl_front_config_.green_valve_flow = task_yaml["task"]["tpl_front"]["green_valve_flow"].as<double>();
    }
    if (task_yaml["task"]["tpl_front"]["blue_valve_flow"])
    {
      tpl_front_config_.blue_valve_flow = task_yaml["task"]["tpl_front"]["blue_valve_flow"].as<double>();
    }
    if (task_yaml["task"]["tpl_front"]["pto_rpm"])
    {
      tpl_front_config_.pto_rpm = task_yaml["task"]["tpl_front"]["pto_rpm"].as<double>();
    }

    if (task_yaml["task"]["tpl_back"]["tpl_angle"])
    {
      tpl_back_config_.tpl_angle = task_yaml["task"]["tpl_back"]["tpl_angle"].as<double>();
    }
    if (task_yaml["task"]["tpl_back"]["tpl_height"])
    {
      tpl_back_config_.tpl_height = task_yaml["task"]["tpl_back"]["tpl_height"].as<double>();
    }
    if (task_yaml["task"]["tpl_back"]["red_valve_flow"])
    {
      tpl_back_config_.red_valve_flow = task_yaml["task"]["tpl_back"]["red_valve_flow"].as<double>();
    }
    if (task_yaml["task"]["tpl_back"]["green_valve_flow"])
    {
      tpl_back_config_.green_valve_flow = task_yaml["task"]["tpl_back"]["green_valve_flow"].as<double>();
    }
    if (task_yaml["task"]["tpl_back"]["blue_valve_flow"])
    {
      tpl_back_config_.blue_valve_flow = task_yaml["task"]["tpl_back"]["blue_valve_flow"].as<double>();
    }
    if (task_yaml["task"]["tpl_back"]["pto_rpm"])
    {
      tpl_back_config_.pto_rpm = task_yaml["task"]["tpl_back"]["pto_rpm"].as<double>();
    }
  }
}

void TplHandler::publishTplConfig(std::shared_ptr<ros::Publisher> tpl_front_config_publisher,
                                  std::shared_ptr<ros::Publisher> tpl_back_config_publisher)
{
  tpl_front_config_publisher->publish(tpl_front_config_);
  tpl_back_config_publisher->publish(tpl_back_config_);
}
}  // namespace agribot_behavior
