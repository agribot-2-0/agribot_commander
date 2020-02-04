#include <AppFrameInterpreter.h>

AppFrameInterpreter::AppFrameInterpreter()
{
    command_id=0;
    int i=0;
    while(i<8)
    {
        msg_byte_array[i]=0;
        i++;
    }
   
}

int AppFrameInterpreter::readFrames()
{
    return 0;
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

