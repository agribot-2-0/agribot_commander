#include <AppFrameInterpreter_can.h>

#define ADDR_MASK 0x7FF
#define PROTOCOL_ARC_FRAME 0x035
#define PROTOCOL_HB_FRAME 0x001
#define PROTOCOL_HANDSHAKE_FRAME 0x00A
#define PROTOCOL_MANUALSTEERING 0x015
#define PROTOCOL_AUTOMATIC_STEERING 0x01F
#define PROTOCOL_ERROR_FRAMES 0x0C8
#define PROTOCOL_MAN_ERROR_FRAMES 0x0C9

#define LIN_VELO_SIGN_BIT 0
#define DIRECTION_SIGN_BIT 1
#define RADIUS_BYTE 2
#define LIN_VELO_BYTE 4
#define RADIUS_PARSE_CONST 10
#define LIN_VELO_PARSE_CONST 100

#define MAX_RADIUS 10
#define MIN_RADIUS 1.5

AppFrameInterpreter_can::AppFrameInterpreter_can(char* pchCanChannel)
{
    _CanHandler.iInitialize(pchCanChannel);
    _fManualAgnularVelo=0;
    _fManualLinearVelo=0;
}

void AppFrameInterpreter_can::vReceiveCanFrames()
{
    can_frame message;
    while(_CanHandler.bReceiveFrame(&message))
    {
        _FramesQueue.push(message);
    }
}

AppFrameInterpreter_can::~AppFrameInterpreter_can()
{
    while(!_FramesQueue.empty())
    {
        _FramesQueue.pop();
    }
}

int16_t AppFrameInterpreter_can::i16GetTwoBytes(uint8_t u8StartBit, can_frame CanFrame)
{
    return ((int16_t)CanFrame.data[u8StartBit])<<8 |((int16_t)CanFrame.data[u8StartBit+1]);
}

uint8_t AppFrameInterpreter_can::u8ParseCanFrames()
{
    uint8_t u8Status = 0;
    can_frame message;
    while(!_FramesQueue.empty())
    {
        message = _FramesQueue.front();
        if((PROTOCOL_ARC_FRAME&ADDR_MASK) == (message.can_id&ADDR_MASK))
        {
                int8_t i8VeloSign = message.data[LIN_VELO_SIGN_BIT]==0 ? -1:1;
                int8_t i8DirectionSign = message.data[DIRECTION_SIGN_BIT]==0 ? -1:1;
                _fManualLinearVelo = ((float)(i16GetTwoBytes(LIN_VELO_BYTE,message))*i8VeloSign)/LIN_VELO_PARSE_CONST;
                float fRadius = ((float)(i16GetTwoBytes(RADIUS_BYTE,message))*i8DirectionSign)/RADIUS_PARSE_CONST;
                _fManualAgnularVelo = 0;
                if(std::abs(fRadius) < MAX_RADIUS && std::abs(fRadius) > MIN_RADIUS)
                {
                    _fManualAgnularVelo=_fManualLinearVelo/fRadius;
                }
                u8Status |= RECEIVED_MANUAL_STEERING;
        }
        else if((PROTOCOL_HANDSHAKE_FRAME&ADDR_MASK) == (message.can_id&ADDR_MASK))
        {
            u8Status |= RECEIVED_HANDSHAKE;
        } 
        else if((PROTOCOL_HB_FRAME&ADDR_MASK) == (message.can_id&ADDR_MASK))
        {
            u8Status |= RECEIVED_HEARTBEAT;
        }
        else if((PROTOCOL_MANUALSTEERING&ADDR_MASK) == (message.can_id&ADDR_MASK))
        {
            u8Status |= RECEIVED_MANUAL_MODE_COMF;
        }
        else if((PROTOCOL_AUTOMATIC_STEERING&ADDR_MASK) == (message.can_id&ADDR_MASK))
        {
            u8Status |= RECEIVED_AUTO_STEERING;
        }
        else if((PROTOCOL_ERROR_FRAMES&ADDR_MASK) == (message.can_id&ADDR_MASK))
        {
            u8Status |= RECEIVED_PROTOCOL_ERROR;
        }
        else if((PROTOCOL_MAN_ERROR_FRAMES&ADDR_MASK) == (message.can_id&ADDR_MASK))
        {
            u8Status |= RECEIVED_MANUALSTEERING_ERROR;
        } 
        _FramesQueue.pop();
    }
    return u8Status;
}

float AppFrameInterpreter_can::fGetManualAngularVelo()
{
    return _fManualAgnularVelo;
}

float AppFrameInterpreter_can::fGetManualLinearVelo()
{
    return _fManualLinearVelo;
}