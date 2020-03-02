#pragma once

#include <agribot_common/Can.h>
#include <queue>
#include <cmath>

#define RECEIVED_HANDSHAKE 0x01
#define RECEIVED_HEARBEAT 0x02
#define RECEIVED_MANUAL_MODE_COMF 0x04
#define RECEIVED_MANUAL_STEERING 0x08
#define RECEIVED_AUTO_STEERING 0x10
#define RECEIVED_PROTOCOL_ERROR 0x20
#define RECEIVED_MANUALSTEERING_ERROR 0x40

class AppFrameInterpreter_can
{
    Can _CanHandler;
    std::queue<can_frame> _FramesQueue;
    double _fManualAgnularVelo = 0;
    double _fManualLinearVelo = 0;
    int16_t i16GetTwoBytes(uint8_t u8StartBit, can_frame CanFrame);
public:
    AppFrameInterpreter_can(char* pchCanChannel);
    ~AppFrameInterpreter_can();
    void vReceiveCanFrames();
    uint8_t u8ParseCanFrames();
    float fGetManualAngularVelo();
    float fGetManualLinearVelo();
};