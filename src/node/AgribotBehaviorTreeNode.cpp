#include <agribot_behavior/node/AgribotBehaviorTreeNode.h>

namespace agribot_behavior
{
AgribotBehaviorTreeNode::AgribotBehaviorTreeNode()
{
  factory.registerNodeType<agribot_behavior::CheckStartConditions>("CheckStartConditions");
  // factory.registerSimpleCondition("CheckBattery", std::bind(CheckBattery));
  // GripperInterface gripper;
  // factory.registerSimpleAction("OpenGripper", std::bind(&GripperInterface::open, &gripper));
  // factory.registerSimpleAction("CloseGripper", std::bind(&GripperInterface::close, &gripper));

  tree = factory.createTreeFromFile("./my_tree.xml");
}

void AgribotBehaviorTreeNode::run()
{
    tree.tickRoot();
}

}  // namespace agribot_behavior
