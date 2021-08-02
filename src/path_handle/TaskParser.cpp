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

void TaskParser::setTaskLocation(const std::experimental::filesystem::path& task_name)
{
  task_location_ = task_folder_ / task_name;
  task_location_ += ".yaml";
}

std::experimental::filesystem::path TaskParser::getPathNameFromTask()
{
  if (fileExists(task_location_))
  {
    YAML::Node task_yaml = YAML::LoadFile(task_location_);
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

bool TaskParser::setPathName()
{
  std::experimental::filesystem::path path_name = getPathNameFromTask();
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

void TaskParser::parseTpl(tplLocation tpl)
{
  std::string which_tpl = "";
  agribot_msgs::AgribotTool tpl_config;
  if (tpl == tplLocation::front)
  {
    which_tpl = "front";
  }
  else if (tpl == tplLocation::back)
  {
    which_tpl = "back";
  }

  if (fileExists(task_location_))
  {
    bool is_all_data_set = true;
    YAML::Node task_yaml = YAML::LoadFile(task_location_);

    if (task_yaml["task"]["tpl"][which_tpl]["angle"])
    {
      tpl_config.tpl_angle = task_yaml["task"]["tpl"][which_tpl]["angle"].as<double>();
    }
    else
    {
      is_all_data_set = false;
    }
    if (task_yaml["task"]["tpl"][which_tpl]["height"])
    {
      tpl_config.tpl_height = task_yaml["task"]["tpl"][which_tpl]["height"].as<double>();
    }
    else
    {
      is_all_data_set = false;
    }
    if (task_yaml["task"]["tpl"][which_tpl]["red_valve_flow"])
    {
      tpl_config.red_valve_flow = task_yaml["task"]["tpl"][which_tpl]["red_valve_flow"].as<double>();
    }
    else
    {
      is_all_data_set = false;
    }
    if (task_yaml["task"]["tpl"][which_tpl]["green_valve_flow"])
    {
      tpl_config.green_valve_flow = task_yaml["task"]["tpl"][which_tpl]["green_valve_flow"].as<double>();
    }
    else
    {
      is_all_data_set = false;
    }
    if (task_yaml["task"]["tpl"][which_tpl]["blue_valve_flow"])
    {
      tpl_config.blue_valve_flow = task_yaml["task"]["tpl"][which_tpl]["blue_valve_flow"].as<double>();
    }
    else
    {
      is_all_data_set = false;
    }
    if (task_yaml["task"]["tpl"][which_tpl]["pto_rpm"])
    {
      tpl_config.pto_rpm = task_yaml["task"]["tpl"][which_tpl]["pto_rpm"].as<double>();
    }
    else
    {
      is_all_data_set = false;
    }

    if (is_all_data_set)
    {
      if (tpl == tplLocation::front)
      {
        tpl_front_config_ = tpl_config;
      }
      else if (tpl == tplLocation::back)
      {
        tpl_back_config_ = tpl_config;
      }
    }
    else
    {
      throw std::invalid_argument("There is missing data in .yaml config file.");
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

void TaskParser::parseTask(const std::experimental::filesystem::path& task_name)
{
  setTaskLocation(task_name);
  parseTpl(tplLocation::front);
  parseTpl(tplLocation::back);
  setPathName();
}

}  // namespace agribot_behavior
