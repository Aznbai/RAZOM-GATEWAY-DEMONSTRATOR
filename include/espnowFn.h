
// ESP-NOW handling

void onReceive(const uint8_t *mac_addr, const uint8_t *incomingData, int len)
{
    memcpy(&currMsg, incomingData, sizeof(currMsg));
    // PROCEED ONLY IF ID IN RANGE
    if (currMsg.id > -1 && currMsg.id < 4)
    {
        // Update the structure of a corresponding  node with data from incoming message
        nodesStruct[currMsg.id].anX = currMsg.anX;
        nodesStruct[currMsg.id].anY = currMsg.anY;
        nodesStruct[currMsg.id].anZ = currMsg.anZ;
        nodesStruct[currMsg.id].acX = currMsg.acX;
        nodesStruct[currMsg.id].acY = currMsg.acY;
        nodesStruct[currMsg.id].acZ = currMsg.acZ;
        nodesStruct[currMsg.id].gyX = currMsg.gyX;
        nodesStruct[currMsg.id].gyY = currMsg.gyY;
        nodesStruct[currMsg.id].gyZ = currMsg.gyZ;
        nodesStruct[currMsg.id].aaX = currMsg.aaX;
        nodesStruct[currMsg.id].aaY = currMsg.aaY;
        //
        // char macStr[18];
        // snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
        //          mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        // Serial.print(">ESP-NOW MSG FROM:");
        // Serial.println(macStr);
        // //
        // Serial.print("HANDLING Core");
        // Serial.println(xPortGetCoreID());
        // Serial.print(">Board ID :");
        // Serial.println(currMsg.id);
        // Serial.print(">Bytes    :");
        // Serial.println(len);
        // // ANGLE
        // Serial.print(">AngleX   :");
        // Serial.println(currMsg.anX);
        // Serial.print(">AngleY   :");
        // Serial.println(currMsg.anY);
        // Serial.print(">AngleZ   :");
        // Serial.println(currMsg.anZ);
        // // ACCELERATION
        // Serial.print(">AccX     :");
        // Serial.println(currMsg.acX);
        // Serial.print(">AccY     :");
        // Serial.println(currMsg.acY);
        // Serial.print(">AccZ     :");
        // Serial.println(currMsg.acZ);
        // // GYRO
        // Serial.print(">GyroX    :");
        // Serial.println(currMsg.gyX);
        // Serial.print(">GyroY    :");
        // Serial.println(currMsg.gyY);
        // Serial.print(">GyroZ    :");
        // Serial.println(currMsg.gyZ);
        // // PITCH / ROLL
        // Serial.print(">Roll     :");
        // Serial.println(currMsg.aaX);
        // Serial.print(">Pitch   :");
        // Serial.println(currMsg.aaY);
    }
}