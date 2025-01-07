#include "PTZProtocolHandler.h"

PTZProtocolHandler::PTZProtocolHandler(Stream* ser) : _ser(ser), bufferIndex(0) {}

void PTZProtocolHandler::begin() {
    memset(buffer, 0, sizeof(buffer));
    bufferIndex = 0;
}

bool PTZProtocolHandler::parseCommand(PTZCommand& command) {
    while (_ser->available()) {
        uint8_t byte = _ser->read();

        // Add byte to buffer
        if (bufferIndex < sizeof(buffer)) {
            buffer[bufferIndex++] = byte;
        } else {
            shiftBuffer();
            buffer[sizeof(buffer) - 1] = byte;
        }

        // Check if buffer contains a valid command
        if (isValidDahuaCommand()) {
            fillDahuaCommand(command);
            return true;
        } else if (isValidPelcoDCommand()) {
            fillPelcoDCommand(command);
            return true;
        } else if (isValidPelcoPCommand()) {
            fillPelcoPCommand(command);
            return true;
        } else if (isValidHikvisionCommand()) {
            fillHikvisionCommand(command);
            return true;
        } else if (isValidHanbangCommand()) {
            fillHanbangCommand(command);
            return true;
        }
    }

    return false; // No valid command found
}

void PTZProtocolHandler::shiftBuffer() {
    memmove(buffer, buffer + 1, sizeof(buffer) - 1);
    bufferIndex--;
}

// Dahua Protocol
bool PTZProtocolHandler::isValidDahuaCommand() {
    if (bufferIndex < 8) return false;
    if (buffer[0] != 0x90) return false; // Check header
    uint8_t checksum = 0;
    for (int i = 0; i < 7; i++) checksum += buffer[i];
    return (checksum & 0xFF) == buffer[7];
}

void PTZProtocolHandler::fillDahuaCommand(PTZCommand& command) {
    command.addr = buffer[1];
    command.command = buffer[2];
    command.action = buffer[3];
    command.data1 = buffer[4];
    command.data2 = buffer[5];
    command.valid = true;
    command.protocol = "Dahua";
    memcpy(command.packet, buffer, sizeof(buffer));
    bufferIndex = 0;
}

// Pelco-D Protocol
bool PTZProtocolHandler::isValidPelcoDCommand() {
    if (bufferIndex < 7) return false;
    if (buffer[0] != 0xFF) return false; // Check header
    uint8_t checksum = 0;
    for (int i = 1; i < 6; i++) checksum += buffer[i];
    return (checksum & 0xFF) == buffer[6];
}

void PTZProtocolHandler::fillPelcoDCommand(PTZCommand& command) {
    command.addr = buffer[1];
    command.command = buffer[3];
    command.action = 0; // Pelco-D doesn't differentiate actions
    command.data1 = buffer[4];
    command.data2 = buffer[5];
    command.valid = true;
    command.protocol = "Pelco-D";
    memcpy(command.packet, buffer, sizeof(buffer));
    bufferIndex = 0;
}

// Pelco-P Protocol
bool PTZProtocolHandler::isValidPelcoPCommand() {
    if (bufferIndex < 8) return false;
    if (buffer[0] != 0xA0) return false; // Check header
    uint8_t checksum = 0;
    for (int i = 0; i < 7; i++) checksum += buffer[i];
    return (checksum & 0xFF) == buffer[7];
}

void PTZProtocolHandler::fillPelcoPCommand(PTZCommand& command) {
    command.addr = buffer[1];
    command.command = buffer[3];
    command.action = 0; // Pelco-P doesn't differentiate actions
    command.data1 = buffer[4];
    command.data2 = buffer[5];
    command.valid = true;
    command.protocol = "Pelco-P";
    memcpy(command.packet, buffer, sizeof(buffer));
    bufferIndex = 0;
}

// Hikvision Protocol
bool PTZProtocolHandler::isValidHikvisionCommand() {
    if (bufferIndex < 8) return false;
    if (buffer[0] != 0xAF) return false; // Check header
    uint8_t checksum = 0;
    for (int i = 0; i < 7; i++) checksum ^= buffer[i];
    return checksum == buffer[7];
}

void PTZProtocolHandler::fillHikvisionCommand(PTZCommand& command) {
    command.addr = buffer[1];
    command.command = buffer[3];
    command.action = 0;
    command.data1 = buffer[4];
    command.data2 = buffer[5];
    command.valid = true;
    command.protocol = "Hikvision";
    memcpy(command.packet, buffer, sizeof(buffer));
    bufferIndex = 0;
}

// Hanbang Protocol
bool PTZProtocolHandler::isValidHanbangCommand() {
    if (bufferIndex < 8) return false;
    if (buffer[0] != 0xB0) return false; // Check header
    uint8_t checksum = 0;
    for (int i = 0; i < 7; i++) checksum += buffer[i];
    return (checksum & 0xFF) == buffer[7];
}

void PTZProtocolHandler::fillHanbangCommand(PTZCommand& command) {
    command.addr = buffer[1];
    command.command = buffer[2];
    command.action = 0;
    command.data1 = buffer[4];
    command.data2 = buffer[5];
    command.valid = true;
    command.protocol = "Hanbang";
    memcpy(command.packet, buffer, sizeof(buffer));
    bufferIndex = 0;
}
