#include <PTZProtocolHandler.h>

PTZProtocolHandler ptz(&Serial2); 

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  ptz.begin();
}

void loop() {
    PTZCommand command;
    if (ptz.parseCommand(command)) {
        if (command.valid) {
            Serial.print("Packet size: ");
            Serial.println(sizeof(command.packet));
            Serial.print("Packet: ");
            for (int i = 0; i < sizeof(command.packet); i++) {Serial.print(command.packet[i], HEX);  Serial.print(" ");}
            Serial.println();
            Serial.print("Protocol: ");
            Serial.println(command.protocol);
            Serial.print("Address: ");
            Serial.println(command.addr);
            Serial.print("Command: ");
            Serial.println(command.command);
            Serial.print("Action: ");
            Serial.println(command.action);
            Serial.print("Data1: ");
            Serial.println(command.data1);
            Serial.print("Data2: ");
            Serial.println(command.data2);
            Serial.println();
        }
    }
}
