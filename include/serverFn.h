

void statusResponseFn(AsyncWebServerRequest *request, JsonVariant &json)
{
    StaticJsonDocument<4096> statusData;
    for (byte i = 0; i < 4; i++)
    {
        // String objectName ="node" + String(i) ; // Create a string for the object name, e.g., "node1", "node2", etc.
        JsonObject node = statusData.createNestedObject("node" + String(i)); // Create a string for the object name, e.g., "node1", "node2", etc.
        // GLOBAL
        node["id"] = nodesStruct[i].id;
        node["mode"] = nodesStruct[i].mode;
        node["fps"] = nodesStruct[i].fps;
        node["active"] = nodesStruct[i].active;
        // MAPPING RANGES
        node["mapSourceMin"] = nodesStruct[i].mapSourceMin;
        node["mapSourceMax"] = nodesStruct[i].mapSourceMax;
        node["mapTargetMin"] = nodesStruct[i].mapTargetMin;
        node["mapTargetMax"] = nodesStruct[i].mapTargetMax;
        // PITCH
        node["pitchCH"] = nodesStruct[i].pitchCH;
        node["pitchCC"] = nodesStruct[i].pitchCC;
        // ROLL
        node["rollCH"] = nodesStruct[i].rollCH;
        node["rollCC"] = nodesStruct[i].rollCC;
        // ACCEL X
        node["accelXCH"] = nodesStruct[i].accelXCH;
        node["accelXCC"] = nodesStruct[i].accelXCC;
        // ACCEL Y
        node["accelYCH"] = nodesStruct[i].accelYCH;
        node["accelYCC"] = nodesStruct[i].accelYCC;
        // ACCEL Z
        node["accelZCH"] = nodesStruct[i].accelZCH;
        node["accelZCC"] = nodesStruct[i].accelZCC;
        // Z-KNOB
        node["zKnobCH"] = nodesStruct[i].zKnobCH;
        node["zKnobCC"] = nodesStruct[i].zKnobCC;
    }
    String postStatusResponse;
    serializeJson(statusData, postStatusResponse);
    request->send(200, "application/json", postStatusResponse);
    Serial.println("post status response output:");
    Serial.println(postStatusResponse);
}
// struct_message contains different data types ( byte, int, String, bool)single function cant handle all these types due to strong type system of C++ .
// we create several updateParameter functions, each overloaded to handle a different data type
// this way a clean interface accommodating the different types is established
void updateParameter(byte &field, int value)
{
    field = static_cast<byte>(value);
}
void updateParameter(int &field, int value)
{
    field = value;
}
void updateParameter(bool &field, int value)
{
    field = static_cast<bool>(value);
}
void updateParameter(String &field, const String &value)
{
    field = value;
}
void setParamResponseFn(AsyncWebServerRequest *request, JsonVariant &json)
{
    if (json.is<JsonObject>())
    {
        JsonObject jsonObj = json.as<JsonObject>();
        int nodeIndex = jsonObj["node"].as<int>(); // extract node number
        const char *param = jsonObj["param"];      // extract parameter name
        int value = jsonObj["value"];              // extract parameter value. will need to handle different value types
        Serial.println("Parameter Update Received: ");
        Serial.print("Node: ");
        Serial.println(nodeIndex);
        Serial.print("Parameter name: ");
        Serial.println(param);
        Serial.print("Parameter Value: ");
        Serial.println(value);

        if (nodeIndex >= 0 && nodeIndex < 4)
        {
            struct_nodeAccumulator &node = nodesStruct[nodeIndex]; // reference the right node
            // Create a new JSON document to hold the response
            StaticJsonDocument<1024> responseJson;
            // determine whicht parameter name was sent
            if (strcmp(param, "mode") == 0)
            {
                updateParameter(node.mode, value);
                responseJson["mode"] = node.mode;
            }
            else if (strcmp(param, "fps") == 0)
            {
                updateParameter(node.fps, value);
                responseJson["fps"] = node.fps;
            }
            else if (strcmp(param, "active") == 0)
            {
                updateParameter(node.active, value);
                responseJson["active"] = node.active;
            }
            else if (strcmp(param, "mapSourceMin") == 0)
            {
                updateParameter(node.mapSourceMin, value);
                responseJson["mapSourceMin"] = node.mapSourceMin;
            }
            else if (strcmp(param, "mapSourceMax") == 0)
            {
                updateParameter(node.mapSourceMax, value);
                responseJson["mapSourceMax"] = node.mapSourceMax;
            }
            else if (strcmp(param, "mapTargetMin") == 0)
            {
                updateParameter(node.mapTargetMin, value);
                responseJson["mapTargetMin"] = node.mapTargetMin;
            }
            else if (strcmp(param, "mapTargetMax") == 0)
            {
                updateParameter(node.mapTargetMax, value);
                responseJson["mapTargetMax"] = node.mapTargetMax;
            }
            else if (strcmp(param, "pitchCH") == 0)
            {
                updateParameter(node.pitchCH, value);
                responseJson["pitchCH"] = node.pitchCH;
            }
            else if (strcmp(param, "pitchCC") == 0)
            {
                updateParameter(node.pitchCC, value);
                responseJson["pitchCC"] = node.pitchCC;
            }
            else if (strcmp(param, "rollCH") == 0)
            {
                updateParameter(node.rollCH, value);
                responseJson["rollCH"] = node.rollCH;
            }
            else if (strcmp(param, "rollCC") == 0)
            {
                updateParameter(node.rollCC, value);
                responseJson["rollCC"] = node.rollCC;
            }
            else if (strcmp(param, "accelXCH") == 0)
            {
                updateParameter(node.accelXCH, value);
                responseJson["accelXCH"] = node.accelXCH;
            }
            else if (strcmp(param, "accelXCC") == 0)
            {
                updateParameter(node.accelXCC, value);
                responseJson["accelXCC"] = node.accelXCC;
            }
            else if (strcmp(param, "accelYCH") == 0)
            {
                updateParameter(node.accelYCH, value);
                responseJson["accelYCH"] = node.accelYCH;
            }
            else if (strcmp(param, "accelYCC") == 0)
            {
                updateParameter(node.accelYCC, value);
                responseJson["accelYCC"] = node.accelYCC;
            }
            else if (strcmp(param, "accelZCH") == 0)
            {
                updateParameter(node.accelZCH, value);
                responseJson["accelZCH"] = node.accelZCH;
            }
            else if (strcmp(param, "accelZCC") == 0)
            {
                updateParameter(node.accelZCC, value);
                responseJson["accelZCC"] = node.accelZCC;
            }
            else if (strcmp(param, "zKnobCH") == 0)
            {
                updateParameter(node.zKnobCH, value);
                responseJson["zKnobCH"] = node.zKnobCH;
            }
            else if (strcmp(param, "zKnobCC") == 0)
            {
                updateParameter(node.zKnobCC, value);
                responseJson["zKnobCC"] = node.zKnobCC;
            }
            // Add remaining node data to the response JSON
            responseJson["node"] = nodeIndex; // Adjust index for response
            // Serialize and send the response
            String response;
            serializeJson(responseJson, response);
            request->send(200, "application/json", response);
            Serial.println("Response sent with updated parameters:");
            Serial.println(response);
        }
        else
        {
            Serial.println("Node index out of range");
        }
    }
    else
    {
        Serial.println(" POST request payload is not a JSON Object! ");
    }
}
void notFoundFn(AsyncWebServerRequest *request)
{
    request->send(404, "application/json", "{\"404\":\"something went 'orribly wrong\"}");
}
