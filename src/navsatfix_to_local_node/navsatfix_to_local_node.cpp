/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */

#include <agribot_commander/NavSatFixToLocalNode.h>

int main(int argc, char** argv)
{
  ros::init(argc, argv, "navsatfix_to_local_node");
  NavSatFixToLocalNode _navSatFixToLocalNode;
  _navSatFixToLocalNode.run();
  return 0;
}
