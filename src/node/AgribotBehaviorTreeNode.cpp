/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/node/AgribotBehaviorTreeNode.h>

#include <iostream>  // for debug

namespace agribot_behavior
{
void AgribotBehaviorTreeNode::registerAllNodes()
{
  factory.registerNodeType<agribot_behavior::CheckStartConditions>("CheckStartConditions");
  factory.registerNodeType<agribot_behavior::SwitchControlArbiters>("SwitchControlArbiters");
  factory.registerNodeType<agribot_behavior::Handle>("Handle");
  // factory.registerSimpleCondition("CheckBattery", std::bind(CheckBattery));
  // GripperInterface gripper;
  // factory.registerSimpleAction("OpenGripper", std::bind(&GripperInterface::open, &gripper));
  // factory.registerSimpleAction("CloseGripper", std::bind(&GripperInterface::close, &gripper));
}

std::string AgribotBehaviorTreeNode::getTreeLocation() const
{
  return ros::package::getPath("agribot_commander") + "/behavior_tree/agribot_behavior.xml";
}

AgribotBehaviorTreeNode::AgribotBehaviorTreeNode(int argc, char** argv)
{
  ros::init(argc, argv, "agribot_tools_control_node");
  node_handle_ = std::make_shared<ros::NodeHandle>();

  registerAllNodes();
  tree = factory.createTreeFromFile(getTreeLocation());
}

void AgribotBehaviorTreeNode::run()
{
  ros::Rate rate(1);
  while (ros::ok())
  {
    tree.tickRoot();
    ros::spinOnce();
    rate.sleep();
  }
}

}  // namespace agribot_behavior
