#pragma once

#include <agribot_common/AgribotComponent.h>
#include <agribot_msgs/AgribotMobileAppFrame.h>
#include <boost/array.hpp>
#include <string>

class AppFrameInterpreter: public AgribotComponent
{
    private:
        bool fresh_data;
        int command_id;
        int8_t msg_byte_array[8];
        std::string chosen_task; 

        float front_linear;
        float front_angular;
        float back_linear;
        float back_angular;

    public:

        AppFrameInterpreter();

        void createStateMachineAppFrame(agribot_msgs::AgribotMobileAppFrame* frame);

        void setAppFrame(const uint32_t rec_frame_ID, const boost::array<unsigned char, 8> rec_bytes);
        void setMoveFrame(float fr_lin, float fr_ang, float bk_lin, float bk_ang);
        void setChosenTask(std::string task_name);

        virtual int configure();
        virtual bool startup();
        virtual bool update();
        virtual bool stop();
        virtual bool cleanup();

        //return command identificator
        int igetCommandID();
        //returns pointer to constant-size (8 bytes) data array
        int8_t* ipgetByteArray();
        //returns name 
        std::string strgetTaskName();
        //check if new frame has been received
        bool dataPending();
};