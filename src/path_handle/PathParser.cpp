/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/path_handle/PathParser.h>

namespace agribot_behavior
{
inline bool PathParser::fileExists(const std::string& name)
{
  std::ifstream f(name.c_str());
  return f.good();
}

void PathParser::setPose(const YAML::Node& node, std::size_t index,
                         std::vector<agribot_msgs::NavSatFixOrientation>& path_points)
{
  path_points[index].orientation.x = node["orientation"][0].as<double>();
  path_points[index].orientation.y = node["orientation"][1].as<double>();
  path_points[index].orientation.z = node["orientation"][2].as<double>();
  path_points[index].orientation.w = node["orientation"][3].as<double>();
  path_points[index].latitude = node["position"][0].as<double>();
  path_points[index].longitude = node["position"][1].as<double>();
  path_points[index].altitude = node["position"][2].as<double>();
}

bool PathParser::setPathName(const std::string& name)
{
  std::string path_location = std::getenv("AGRIBOT_HOME");
  path_location += "/agribot/configs/base/map/paths/paths/";
  path_location += name;
  if (fileExists(path_location))
  {
    path_location_ = path_location;
    return true;
  }
  return false;
}

std::vector<agribot_msgs::NavSatFixOrientation> PathParser::parsePath()
{
  if (path_location_ != "")
  {
    std::vector<agribot_msgs::NavSatFixOrientation> path_points;
    YAML::Node path_points_yaml = YAML::LoadFile(path_location_);
    path_points.clear();
    path_points.resize(path_points_yaml.size());
    for (std::size_t index = 0; index < path_points_yaml.size(); ++index)
    {
      setPose(path_points_yaml["pose" + std::to_string(index)], index, path_points);
    }
    return path_points;
  }
  return std::vector<agribot_msgs::NavSatFixOrientation>();
}

}  // namespace agribot_behavior
