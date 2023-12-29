void initSerial()
{
    Serial.begin(115200);
    Serial.println("");
    Serial.println("");
    Serial.println("");
    Serial.println("SERIAL 0 OK");
    Serial.println("STARTING SETUP");
}
void initMidi()
{
    Serial2.begin(31250);
    midi2.begin(MIDI_CHANNEL_OMNI);
    Serial.println("MIDI ON SERIAL#2 OK");
}
void initBtn()
{ // ACTIVATE PULLUPS AT  BUTTON PINS
    pinMode(4, INPUT_PULLUP);
    pinMode(19, INPUT_PULLUP);
    Serial.println("BUTTONS OK");
}
void initLED()
{                                                     // PWM LED INIT
    ledcSetup(PWMledChannel, PWMfreq, PWMresolution); // configure LED PWM functionalitites
    ledcAttachPin(LED_BUILTIN, PWMledChannel);        // attach the channel to the GPIO to be
    // pinMode(LED_BUILTIN, OUTPUT); // pin 2 on ESP32/ESP32-WROOM
    for (byte i = 0; i < 255; i++)
    {
        ledcWrite(PWMledChannel, i);
        delay(1);
    }
    for (byte i = 255; i > 1; i--)
    {
        ledcWrite(PWMledChannel, i);
        delay(1);
    }
    Serial.println("LED OK");
}

// Initialize SPIFFS
void initFS()
{
    if (!SPIFFS.begin())
    {
        Serial.println("SPIFFS MOUNT ERROR!");
    }
    else
    {
        Serial.println("SPIFFS MOUNT OK");
    }
}
void initWifi()
{ // WORKING WIFI SETUP OF GET-POST-API EXAMPLE
  // WiFi.mode(WIFI_STA);
  // WiFi.begin(ssid, password);
  // while (WiFi.waitForConnectResult() != WL_CONNECTED)
  // {
  //     Serial.prinln("WIFI CONNECTION FAILED!");
  // }
  // Serial.println("WIFI OK");
  // Serial.print("LOCAL IP ADDRESS: ");
  // Serial.println(WiFi.localIP());
    WiFi.mode(WIFI_MODE_APSTA);
    WiFi.begin(ssid, password);
    Serial.print("CONNECTING TO WIFI: ");
    Serial.print(ssid);
    Serial.println(" ");

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(100);
    }
    Serial.println(" ");
    Serial.println("WIFI CONNECTED");
    WiFi.setSleep(false); // TEST USAGE

    IPAddress ip = WiFi.localIP();
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("WIFI CHANNEL: ");
    Serial.println(WiFi.channel());
    Serial.print("LOCAL IP: ");
    Serial.println(WiFi.localIP());

    //   Serial.print("WIFI CONNECTION ESTABLISHED");
    //   Serial.print("LOCAL IP Address: ");
    //   Serial.println(WiFi.localIP());
    //   Serial.print("Wi-Fi Channel: ");
    //   Serial.println(WiFi.channel());
}
void initEspNow()
{
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ESP-NOW INIT ERROR");
        while (1)
            ;
    }
    esp_now_register_recv_cb(onReceive);
    Serial.println("ESP-NOW INIT OK");
    Serial.println("RX CALLBACK REGISTRATION OK");
    for (byte i = 255; i > 1; i--)
    {
        ledcWrite(PWMledChannel, i);
        delay(1);
    }
}
void initDNS()
{
    if (!MDNS.begin(domainName))
    {
        Serial.println("mDNS INIT ERROR");
        return;
    }
    else
    {
        Serial.println("mDNS OK");
        Serial.println("WEB INTERFACE AVAILABLE AT:");
        Serial.print("http://");
        Serial.print(domainName);
        Serial.println(".local");
    }
}
void initServerRoutes()
{
    server.onNotFound(notFoundFn);
    //
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html", "text/html"); });
    server.serveStatic("/", SPIFFS, "/");
    Serial.println("STATIC HOST OK");
    // SET PARAMETER POST REQUEST HANDLER
    AsyncCallbackJsonWebHandler *setParamHandler =
        new AsyncCallbackJsonWebHandler("/set-param", setParamResponseFn);
    server.addHandler(setParamHandler);
    Serial.println("PARAMETER SET: POST REQ. HANDLER OK");
    // GET STATUS POST REQUEST HANDLER
    AsyncCallbackJsonWebHandler *statusHandler =
        new AsyncCallbackJsonWebHandler("/status", statusResponseFn);
    server.addHandler(statusHandler);
    Serial.println("STATUS GET: POST REQ. HANDLER OK");
    //
    server.begin();
    Serial.println("SERVER START OK");
}