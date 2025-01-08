#include "PTZProtocolHandler.h"

PTZProtocolHandler::PTZProtocolHandler(Stream* ser) : _ser(ser), bufferIndex(0) {}

void PTZProtocolHandler::begin() {
    memset(buffer, 0, sizeof(buffer));
    bufferIndex = 0;
}

bool PTZProtocolHandler::parseCommand(PTZCommand& command) {
    if (10 < (unsigned long)(millis() - _timer)) bufferIndex = 0;
    while (_ser->available()) {
	if (bufferIndex > 10) bufferIndex = 0;
        uint8_t byte = _ser->read();	
	bool isByteOk = true;
	if (bufferIndex == 0 && !(byte == 0x90 || byte == 0xFF || byte == 0xA0 || byte == 0xE1 || byte == 0xF6)) isByteOk = false;
	if (isByteOk){

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
	_timer = millis();
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
    command.command = buffer[2];	//Command 1
    command.action = buffer[3]; 	//Command 2
    command.data1 = buffer[4];		//Data 1
    command.data2 = buffer[5];		//Data 2
    command.valid = true;
    command.protocol = "Pelco-D";
    memcpy(command.packet, buffer, sizeof(buffer));
    bufferIndex = 0;
}

// Pelco-P Protocol
bool PTZProtocolHandler::isValidPelcoPCommand() {
    if (bufferIndex < 8) return false;
    if (buffer[0] != 0xA0 && buffer[6] != 0xAF) return false; // Check header
    uint8_t checksum = 0;
    for (int i = 0; i < 7; i++) checksum ^= buffer[i];
    return (checksum & 0xFF) == buffer[7];
}

void PTZProtocolHandler::fillPelcoPCommand(PTZCommand& command) {
    command.addr = buffer[1];
    command.command = buffer[2]; 	//Data 1
    command.action = buffer[3]; 	//Data 2
    command.data1 = buffer[4]; 		//Data 3
    command.data2 = buffer[5];		//Data 4
    command.valid = true;
    command.protocol = "Pelco-P";
    memcpy(command.packet, buffer, sizeof(buffer));
    bufferIndex = 0;
}

// Hikvision Protocol
bool PTZProtocolHandler::isValidHikvisionCommand() {
    if (bufferIndex < 6) return false;
    if (buffer[0] != 0xE1) return false; // Check header
    uint8_t checksum = buffer[5]; // Extracted checksum
    uint8_t calculated = 0;
    for (int i = 0; i < 5; i++) calculated ^= buffer[i];
    return calculated == checksum;
}

void PTZProtocolHandler::fillHikvisionCommand(PTZCommand& command) {
    command.addr = buffer[1];		//?
    command.command = buffer[2];	//?
    command.action = buffer[4]; 	//?
    command.data1 = buffer[3];		//?
    command.data2 = 0; 			// Not used
    command.valid = true;
    command.protocol = "Hikvision";
    memcpy(command.packet, buffer, sizeof(buffer));
    bufferIndex = 0;
}

// Hanbang Protocol
bool PTZProtocolHandler::isValidHanbangCommand() {
    if (bufferIndex < 7) return false;
    if (buffer[0] != 0xF6) return false; // Check header
    uint8_t checksum = buffer[6]; // Extracted checksum
    uint8_t calculated = 0;
    for (int i = 1; i < 6; i++) calculated += buffer[i];
    return (calculated & 0x7F) == checksum;
}

void PTZProtocolHandler::fillHanbangCommand(PTZCommand& command) {
    command.addr = buffer[2];
    command.command = buffer[1]; //?
    command.action = buffer[3];	 //?
    command.data1 = buffer[4];   //?
    command.data2 = buffer[5];	 //?
    command.valid = true;
    command.protocol = "Hanbang";
    memcpy(command.packet, buffer, sizeof(buffer));
    bufferIndex = 0;
}
