/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/node/AgribotBehaviorTreeNode.h>

#include <iostream>  // for debug

namespace agribot_behavior
{
void AgribotBehaviorTreeNode::registerAllNodes()
{
  factory_->registerNodeType<CheckStartConditions>("CheckStartConditions");
  factory_->registerNodeType<SwitchControlArbiters>("SwitchControlArbiters");
  factory_->registerNodeType<Handle>("Handle");
}

std::string AgribotBehaviorTreeNode::getTreeLocation() const
{
  return ros::package::getPath("agribot_commander") + "/behavior_tree/agribot_behavior.xml";
}

void AgribotBehaviorTreeNode::initTreeNodesWithRosDependencies()
{
  for (auto& node : tree_->nodes)
  {
    if (auto switch_control_arbiter = dynamic_cast<SwitchControlArbiters*>(node.get());
        switch_control_arbiter != nullptr)
    {
      switch_control_arbiter->init_tool_arbiter_client(tool_arbiter_client_);
      switch_control_arbiter->init_cmd_vel_arbiter_clients(cmd_vel_manual_client_, cmd_vel_navigation_client_,
                                                           cmd_vel_stop_client_);
    }
  }
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

  // Publishers

  // Subscribers

  // Parameters

  // Services
  tool_arbiter_client_ = std::make_shared<ros::ServiceClient>(  // NOLINT
      node_handle_->serviceClient<agribot_msgs::ToolManagerControlSource>("agribot/tool_management/control_source"));
  cmd_vel_manual_client_ =
    std::make_shared<ros::ServiceClient>(
      node_handle_->serviceClient<std_srvs::Empty>("/cmd_vel_arbiter_node/switch_to_teleop"));
  cmd_vel_navigation_client_ =
    std::make_shared<ros::ServiceClient>(
      node_handle_->serviceClient<std_srvs::Empty>("/cmd_vel_arbiter_node/switch_to_navigation"));
  cmd_vel_stop_client_ =
    std::make_shared<ros::ServiceClient>(
      node_handle_->serviceClient<std_srvs::Empty>("/cmd_vel_arbiter_node/switch_to_stop"));

  initTreeNodesWithRosDependencies();
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
