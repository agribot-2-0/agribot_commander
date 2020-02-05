#include <AppFrameInterpreter.h>
#include <ros/ros.h>
#include <ros/console.h>
#include <iostream>


AppFrameInterpreter::AppFrameInterpreter()
{
    command_id=0;
    fresh_data=false;
    int i=0;
    while(i<8)
    {
        msg_byte_array[i]=20;
        i++;
    }
}

void AppFrameInterpreter::setAppFrame(const uint32_t rec_frame_ID, const boost::array<unsigned char, 8> rec_bytes)
{
    command_id=rec_frame_ID;
    msg_byte_array[0]=rec_bytes[0];
    msg_byte_array[1]=rec_bytes[1];
    msg_byte_array[2]=rec_bytes[2];
    msg_byte_array[3]=rec_bytes[3];
    msg_byte_array[4]=rec_bytes[4];
    msg_byte_array[5]=rec_bytes[5];
    msg_byte_array[6]=rec_bytes[6];
    msg_byte_array[7]=rec_bytes[7];
    ROS_INFO("Received app event ID = [%d]", rec_frame_ID);
    fresh_data=true;
}

//Method used to register and inform state machine about incoming msg
void AppFrameInterpreter::setMoveFrame(float fr_lin, float fr_ang, float bk_lin, float bk_ang)
{
    if (!fresh_data)
    {
        if (fr_lin != 0 && fr_ang != 0 && bk_lin != 0 && bk_ang != 0)
        {
            command_id=53;
            front_linear=fr_lin;
            front_angular=fr_ang;
            back_linear=bk_lin;
            back_angular=bk_ang;
            fresh_data=true;
            ROS_INFO("Received only move commands ID = [%d]", command_id);
        }
    } 
}

void AppFrameInterpreter::createStateMachineAppFrame(agribot_commander::mobile_app_frame* frame)
{
    if (fresh_data)
    {
        frame->header.stamp = ros::Time::now();
        frame->header.frame_id = "mobile_app_commands_link";
        frame->can_id = command_id;
        frame->can_byte_array[0] = msg_byte_array[0];
        frame->can_byte_array[1] = msg_byte_array[1];
        frame->can_byte_array[2] = msg_byte_array[2];
        frame->can_byte_array[3] = msg_byte_array[3];
        frame->can_byte_array[4] = msg_byte_array[4];
        frame->can_byte_array[5] = msg_byte_array[5];
        frame->can_byte_array[6] = msg_byte_array[6];
        frame->can_byte_array[7] = msg_byte_array[7];

        fresh_data = false;
    }
}

int AppFrameInterpreter::igetCommandID()
{
    return command_id;
}

int8_t* AppFrameInterpreter::ipgetByteArray()
{
    return msg_byte_array;
}

int AppFrameInterpreter::configure()
{
    return 0;
}

bool AppFrameInterpreter::startup()
{
    return true;
}

bool AppFrameInterpreter::update()
{
    return true;
}

bool AppFrameInterpreter::stop()
{
    return true;
}

bool AppFrameInterpreter::cleanup()
{
    return true;
}

bool AppFrameInterpreter::dataPending()
{
    return fresh_data;
}