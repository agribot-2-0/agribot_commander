/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/path_handle/TaskParser.h>

namespace agribot_behavior
{
TaskParser::TaskParser(const std::experimental::filesystem::path& path_folder,
                       const std::experimental::filesystem::path& task_folder)
{
  path_folder_ = path_folder;
  task_folder_ = task_folder;
}

std::experimental::filesystem::path
TaskParser::getPathNameFromTask(const std::experimental::filesystem::path& task_name)
{
  std::experimental::filesystem::path task_location = task_folder_ / task_name;
  task_location += ".yaml";
  if (fileExists(task_location))
  {
    YAML::Node task_yaml = YAML::LoadFile(task_location);
    if (task_yaml["task"]["path_name"][0])
    {
      return task_yaml["task"]["path_name"][0].as<std::string>() + ".yaml";
    }
  }
  return "";
}

inline bool TaskParser::fileExists(const std::experimental::filesystem::path& name)
{
  std::ifstream f(name.c_str());
  return f.good();
}

void TaskParser::setPose(const YAML::Node& node, std::size_t index, std::vector<sensor_msgs::NavSatFix>& path_points)
{
  path_points[index].latitude = node["position"][0].as<double>();
  path_points[index].longitude = node["position"][1].as<double>();
  path_points[index].altitude = node["position"][2].as<double>();
}

bool TaskParser::setPathName(const std::experimental::filesystem::path& task_name)
{
  std::experimental::filesystem::path path_name = getPathNameFromTask(task_name);
  if (path_name != "")
  {
    std::experimental::filesystem::path path_location = path_folder_ / path_name;
    if (fileExists(path_location))
    {
      path_location_ = path_location;
      return true;
    }
  }
  return false;
}

std::vector<sensor_msgs::NavSatFix> TaskParser::parsePath()
{
  if (path_location_ != "")
  {
    std::vector<sensor_msgs::NavSatFix> path_points;
    YAML::Node path_points_yaml = YAML::LoadFile(path_location_);
    path_points.clear();
    path_points.resize(path_points_yaml.size());
    for (std::size_t index = 0; index < path_points_yaml.size(); ++index)
    {
      setPose(path_points_yaml["pose" + std::to_string(index)], index, path_points);
    }
    return path_points;
  }
  return std::vector<sensor_msgs::NavSatFix>();
}

std::experimental::filesystem::path TaskParser::getPathLocation() const
{
  return path_location_;
}

void TaskParser::parseTpl(const std::experimental::filesystem::path& task_name, whichTpl tpl)
{
  std::string which_tpl = "";
  agribot_msgs::AgribotTool* tpl_config;
  if (tpl == front)
  {
    which_tpl = "front";
    tpl_config = &tpl_front_config_;
  }
  else if (tpl == back)
  {
    which_tpl = "back";
    tpl_config = &tpl_back_config_;
  }

  std::experimental::filesystem::path task_location = task_folder_ / task_name;
  task_location += ".yaml";

  if (fileExists(task_location))
  {
    YAML::Node task_yaml = YAML::LoadFile(task_location);

    if (task_yaml["task"]["tpl"][which_tpl]["angle"])
    {
      tpl_config->tpl_angle = task_yaml["task"]["tpl"][which_tpl]["angle"].as<double>();
    }
    if (task_yaml["task"]["tpl"][which_tpl]["height"])
    {
      tpl_config->tpl_height = task_yaml["task"]["tpl"][which_tpl]["height"].as<double>();
    }
    if (task_yaml["task"]["tpl"][which_tpl]["red_valve_flow"])
    {
      tpl_config->red_valve_flow = task_yaml["task"]["tpl"][which_tpl]["red_valve_flow"].as<double>();
    }
    if (task_yaml["task"]["tpl"][which_tpl]["green_valve_flow"])
    {
      tpl_config->green_valve_flow = task_yaml["task"]["tpl"][which_tpl]["green_valve_flow"].as<double>();
    }
    if (task_yaml["task"]["tpl"][which_tpl]["blue_valve_flow"])
    {
      tpl_config->blue_valve_flow = task_yaml["task"]["tpl"][which_tpl]["blue_valve_flow"].as<double>();
    }
    if (task_yaml["task"]["tpl"][which_tpl]["pto_rpm"])
    {
      tpl_config->pto_rpm = task_yaml["task"]["tpl"][which_tpl]["pto_rpm"].as<double>();
    }
  }
}

agribot_msgs::AgribotTool TaskParser::getTplFrontConfig()
{
  return tpl_front_config_;
}

agribot_msgs::AgribotTool TaskParser::getTplBackConfig()
{
  return tpl_back_config_;
}

}  // namespace agribot_behavior
