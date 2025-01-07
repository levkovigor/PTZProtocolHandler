#ifndef PTZProtocolHandler_h
#define PTZProtocolHandler_h

#include <Arduino.h>

struct PTZCommand {
    uint8_t addr;      // Address of the device
    uint8_t command;   // Command byte
    uint8_t data1;     // Data byte 1
    uint8_t data2;     // Data byte 2
    uint8_t action;    // Action (press/release or movement)
    bool valid;        // Whether the packet is valid
    String protocol;   // Protocol type ("Dahua", "Pelco-D", "Pelco-P", "Hikvision", "Hanbang")
    uint8_t packet[8]; // Source packet data
};

class PTZProtocolHandler {
public:
    PTZProtocolHandler(Stream* ser);
    void begin();
    bool parseCommand(PTZCommand& command);

private:
    Stream* _ser;
    uint8_t buffer[8];
    size_t bufferIndex;

    void shiftBuffer();
    bool isValidDahuaCommand();
    void fillDahuaCommand(PTZCommand& command);
    bool isValidPelcoDCommand();
    void fillPelcoDCommand(PTZCommand& command);
    bool isValidPelcoPCommand();
    void fillPelcoPCommand(PTZCommand& command);
    bool isValidHikvisionCommand();
    void fillHikvisionCommand(PTZCommand& command);
    bool isValidHanbangCommand();
    void fillHanbangCommand(PTZCommand& command);
};

#endif
