/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/node/AgribotBehaviorTreeNode.h>

#include <iostream>  // for debug

namespace agribot_behavior
{
void AgribotBehaviorTreeNode::registerAllNodes()
{
  factory_->registerNodeType<agribot_behavior::CheckStartConditions>("CheckStartConditions");
  factory_->registerNodeType<agribot_behavior::SwitchControlArbiters>("SwitchControlArbiters");
  factory_->registerNodeType<agribot_behavior::Handle>("Handle");
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
  : factory_{ std::make_unique<BT::BehaviorTreeFactory>() }
{
  // ROS init:
  ros::init(argc, argv, "agribot_tools_control_node");
  node_handle_ = std::make_shared<ros::NodeHandle>();

  // Register all custom actions on nodes factory and load tree from XML file
  registerAllNodes();
  tree_ = std::make_unique<BT::Tree>(factory_->createTreeFromFile(getTreeLocation()));

  // Register ZeroMQ publisher for Groot tree view:
  tree_state_publisher_ = std::make_unique<BT::PublisherZMQ>(*tree_);
}

void AgribotBehaviorTreeNode::run()
{
  ros::Rate rate(1);
  while (ros::ok())
  {
    tree_->tickRoot();
    ros::spinOnce();
    rate.sleep();
  }
}

}  // namespace agribot_behavior
