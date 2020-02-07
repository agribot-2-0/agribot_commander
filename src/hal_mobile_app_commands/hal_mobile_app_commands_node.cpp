#include <ros/ros.h>
#include <ros/console.h>
#include <agribot_msgs/AgribotMobileAppFrame.h>
#include <agribot_msgs/AgribotCaterpillar.h>
#include <sstream>
#include <AppFrameInterpreter.h>

AppFrameInterpreter Interpreter;

void app_commands_eventsCallback(const agribot_msgs::AgribotMobileAppFrame::ConstPtr& msg)
{
  Interpreter.setAppFrame(msg->can_id, msg->can_byte_array);
}

void app_move_commandsCallback(const agribot_msgs::AgribotCaterpillar::ConstPtr& msg)
{
  Interpreter.setMoveFrame(msg->front_linear, msg->front_angular, msg->back_linear, msg->back_angular);
}

int main(int argc, char **argv)
{

  ros::init(argc, argv, "hal_mobile_app_commands");
  ros::NodeHandle n;

  ros::Publisher state_machine_pub = n.advertise<agribot_msgs::AgribotMobileAppFrame>("agribot/mobile_app_communication", 1000);
  ros::Subscriber app_commands_events_sub = n.subscribe("/agribot/app_commands_events", 1, app_commands_eventsCallback);
  ros::Subscriber app_move_commands_sub = n.subscribe("/agribot/control/caterpillar", 1, app_move_commandsCallback);

  ros::Rate loop_rate(10);

  agribot_msgs::AgribotMobileAppFrame frame_msg;

  while (ros::ok())
  {

    Interpreter.update();

    if (Interpreter.dataPending())
    {

      Interpreter.createStateMachineAppFrame(&frame_msg);

      ROS_INFO("%s", frame_msg.header.frame_id.c_str());

      state_machine_pub.publish(frame_msg);

    }

    ros::spinOnce();

    loop_rate.sleep();
  }

  return 0;
}