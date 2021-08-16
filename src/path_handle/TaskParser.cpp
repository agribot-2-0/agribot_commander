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

void TaskParser::parseTpl(TplLocation tpl)
{
  std::string which_tpl = convertTplLocationToString(tpl);
  agribot_msgs::AgribotTool tpl_config;

  if (fileExists(task_location_))
  {
    YAML::Node task_yaml = YAML::LoadFile(task_location_);
    const YAML::Node& task = task_yaml["task"]["tpl"][which_tpl];

    if (task["angle"])
    {
      tpl_config.tpl_angle = task["angle"].as<double>();
    }
    else
    {
      throw std::invalid_argument(which_tpl + " tpl angle value is missing");
    }
    if (task["height"])
    {
      tpl_config.tpl_height = task["height"].as<double>();
    }
    else
    {
      throw std::invalid_argument(which_tpl + " tpl height value is missing");
    }
    if (task["red_valve_flow"])
    {
      tpl_config.red_valve_flow = task["red_valve_flow"].as<double>();
    }
    else
    {
      throw std::invalid_argument(which_tpl + " tpl red valve flow value is missing");
    }
    if (task["green_valve_flow"])
    {
      tpl_config.green_valve_flow = task["green_valve_flow"].as<double>();
    }
    else
    {
      throw std::invalid_argument(which_tpl + " tpl green valve flow value is missing");
    }
    if (task["blue_valve_flow"])
    {
      tpl_config.blue_valve_flow = task["blue_valve_flow"].as<double>();
    }
    else
    {
      throw std::invalid_argument(which_tpl + " tpl blue valve flow value is missing");
    }
    if (task["pto_rpm"])
    {
      tpl_config.pto_rpm = task["pto_rpm"].as<double>();
    }
    else
    {
      throw std::invalid_argument(which_tpl + " tpl pto rpm value is missing");
    }

    if (tpl == TplLocation::front)
    {
      tpl_front_config_ = tpl_config;
    }
    else if (tpl == TplLocation::back)
    {
      tpl_back_config_ = tpl_config;
    }
  }
}

void TaskParser::parseWeatherConditions()
{
  if (fileExists(task_location_))
  {
    YAML::Node task_yaml = YAML::LoadFile(task_location_);
    const YAML::Node& task_weather_conditions = task_yaml["task"]["weather_conditions"];

    if (task_weather_conditions["pressure"])
    {
      weather_conditions_from_task_.pressure_.value_ = task_weather_conditions["pressure"]["value"].as<double>();
      weather_conditions_from_task_.pressure_.condition_ =
          convertWeatherConditionToEnumCondition(task_weather_conditions["pressure"]["condition"].as<std::string>());
    }
    else
    {
      throw std::invalid_argument("weather_conditions pressure value is missing.");
    }
    if (task_weather_conditions["humidity"])
    {
      weather_conditions_from_task_.humidity_.value_ = task_weather_conditions["humidity"]["value"].as<double>();
      weather_conditions_from_task_.humidity_.condition_ =
          convertWeatherConditionToEnumCondition(task_weather_conditions["humidity"]["condition"].as<std::string>());
    }
    else
    {
      throw std::invalid_argument("weather_conditions humidity value is missing.");
    }
    if (task_weather_conditions["temperature"])
    {
      weather_conditions_from_task_.temperature_.value_ = task_weather_conditions["temperature"]["value"].as<double>();
      weather_conditions_from_task_.temperature_.condition_ =
          convertWeatherConditionToEnumCondition(task_weather_conditions["temperature"]["condition"].as<std::string>());
    }
    else
    {
      throw std::invalid_argument("weather_conditions atmospheric pressure value is missing");
    }
    if (task_weather_conditions["wind"])
    {
      weather_conditions_from_task_.wind_.value_ = task_weather_conditions["wind"]["value"].as<double>();
      weather_conditions_from_task_.wind_.condition_ =
          convertWeatherConditionToEnumCondition(task_weather_conditions["wind"]["condition"].as<std::string>());
    }
    else
    {
      throw std::invalid_argument("weather_conditions wind speed value is missing.");
    }
  }
}

std::string TaskParser::convertTplLocationToString(TplLocation tpl)
{
  return (tpl == TplLocation::front ? "front" : "back");
}

Condition TaskParser::convertWeatherConditionToEnumCondition(const std::string& condition)
{
  if (condition == "less")
  {
    return Condition::Less;
  }
  else if (condition == "equal")
  {
    return Condition::Equal;
  }
  else if (condition == "more")
  {
    return Condition::More;
  }
  else
  {
    return Condition::None;
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

WeatherConditions TaskParser::getWeatherConditions()
{
  return weather_conditions_from_task_;
}

void TaskParser::parseTask(const std::experimental::filesystem::path& task_name)
{
  setTaskLocation(task_name);
  parseTpl(TplLocation::front);
  parseTpl(TplLocation::back);
  parseWeatherConditions();
  setPathName();
}

}  // namespace agribot_behavior
