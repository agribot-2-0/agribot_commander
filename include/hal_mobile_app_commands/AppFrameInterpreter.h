#pragma once

#include <agribot_common/AgribotComponent.h>

class AppFrameInterpreter: public AgribotComponent
{
    private:
        int command_id;
        int8_t msg_byte_array[8];

    public:
        AppFrameInterpreter();

        int readFrame();

        virtual int configure();
        virtual bool startup();
        virtual bool update();
        virtual bool stop();
        virtual bool cleanup();

        //return command identificator
        int igetCommandID();
        //returns pointer to constant-size (8 bytes) data array
        int8_t* ipgetByteArray();
};