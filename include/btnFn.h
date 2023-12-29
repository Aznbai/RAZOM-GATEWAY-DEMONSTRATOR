void btnRoutine()
{ // BUTTON TEST
    if (digitalRead(4) == LOW)
    {
        Serial.println("BUTTON USER_0 WAS PUSHED!");

        for (byte i = 0; i < 10; i++)
        {
            ledcWrite(PWMledChannel, 255);
            delay(50);
            ledcWrite(PWMledChannel, 0);
            delay(50);
        }
    }
    if (digitalRead(19) == LOW)
    {
        Serial.println("BUTTON USER_1 WAS PUSHED!");

        for (byte i = 0; i < 2; i++)
        {
            ledcWrite(PWMledChannel, 255);
            delay(500);
            ledcWrite(PWMledChannel, 0);
            delay(500);
        }
    }
}