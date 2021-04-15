#pragma once

#include <cstdlib>
//#include <behaviortree_cpp_v3/behavior_tree.h>
#include <behaviortree_cpp_v3/bt_factory.h>
#include <agribot_behavior/action/CheckStartConditions.h>

namespace agribot_behavior
{
class AgribotBehaviorTreeNode
{
private:
  BT::BehaviorTreeFactory factory;
  BT::Tree tree;
public:
  AgribotBehaviorTreeNode();
  void run();
};
}  // namespace agribot_behavior
