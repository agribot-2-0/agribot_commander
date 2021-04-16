/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/node/AgribotBehaviorTreeNode.h>

int main(int argc, char** argv)
{
  agribot_behavior::AgribotBehaviorTreeNode node(argc, argv);
  node.run();
  return 0;
}
