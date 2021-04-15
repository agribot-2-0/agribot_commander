#include <agribot_behavior/node/AgribotBehaviorTreeNode.h>

#include <iostream> //for debug

namespace agribot_behavior
{
void AgribotBehaviorTreeNode::registerAllNodes()
{
  factory.registerNodeType<agribot_behavior::CheckStartConditions>("CheckStartConditions");
  // factory.registerSimpleCondition("CheckBattery", std::bind(CheckBattery));
  // GripperInterface gripper;
  // factory.registerSimpleAction("OpenGripper", std::bind(&GripperInterface::open, &gripper));
  // factory.registerSimpleAction("CloseGripper", std::bind(&GripperInterface::close, &gripper));
}

std::string AgribotBehaviorTreeNode::getTreeLocation() const
{
  return ros::package::getPath("agribot_commander") + "/behavior_tree/agribot_behavior.xml";
}

AgribotBehaviorTreeNode::AgribotBehaviorTreeNode()
{
  registerAllNodes();
  std::cerr << getTreeLocation();
  tree = factory.createTreeFromFile(getTreeLocation());
}

void AgribotBehaviorTreeNode::run()
{
  tree.tickRoot();
}

}  // namespace agribot_behavior
