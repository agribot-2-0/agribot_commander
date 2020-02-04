#include <ros/ros.h>
#include <ros/console.h>
#include <agribot_commander/mobile_app_frame.h>
#include <sstream>
#include <AppFrameInterpreter.h>

int main(int argc, char **argv)
{

  ros::init(argc, argv, "hal_mobile_app_commands");
  ros::NodeHandle n;
  ros::Publisher state_machine_pub = n.advertise<agribot_commander::mobile_app_frame>("agribot/mobile_app_communication", 1000);

  ros::Rate loop_rate(10);
  std::stringstream ss;

  agribot_commander::mobile_app_frame frame_msg;

  AppFrameInterpreter Interpre();

  while (ros::ok())
  {
    //test of the can_message frame
    frame_msg.header.stamp=ros::Time::now();
    frame_msg.header.frame_id = "mobile_app_commands_link";
    frame_msg.can_id=100;
    frame_msg.can_byte_array[0]=10;
    frame_msg.can_byte_array[1]=20;
    frame_msg.can_byte_array[2]=30;
    frame_msg.can_byte_array[3]=40;
    frame_msg.can_byte_array[4]=50;
    frame_msg.can_byte_array[5]=60;
    frame_msg.can_byte_array[6]=70;
    frame_msg.can_byte_array[7]=80;

    ss<<frame_msg.can_id;
    ROS_INFO("%s", frame_msg.header.frame_id.c_str());
  
    state_machine_pub.publish(frame_msg);

    ros::spinOnce();

    loop_rate.sleep();
  }

  return 0;
}