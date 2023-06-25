#include "wled.h"
#include "WString.h"

/*
 * This v1 usermod file allows you to add own functionality to WLED more easily
 * See: https://github.com/Aircoookie/WLED/wiki/Add-own-functionality
 * EEPROM bytes 2750+ are reserved for your custom use case. (if you extend #define EEPSIZE in const.h)
 * If you just need 8 bytes, use 2551-2559 (you do not need to increase EEPSIZE)
 *
 * Consider the v2 usermod API if you need a more advanced feature set!
 */

#define UTC_OFFSET "utcoffset"
char s[50];
char output[1536];
bool enable[10] = {true, true, true, true, true, true, true, true, true, true};

static void GetUtcOffset()
{
    memset(s, 0, strlen(s));
    String utc = String("{\"utcoffset\":") + String(utcOffsetSecs) + String("}");
    strcpy_P(s, utc.c_str());
}

static void UtcOffsetFeature()
{
    server.on("/settings/time/utcoffset", HTTP_GET, [](AsyncWebServerRequest *request)
              { 
                GetUtcOffset();
                request->send(200, "text/plain", s); });

    AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler(
        "/settings/time/utcoffset", [](AsyncWebServerRequest *request)
        {
            bool verboseResponse = false;
            if (!requestJSONBufferLock(22))
                return;
            DeserializationError error = deserializeJson(doc, (uint8_t *)(request->_tempObject));
            JsonObject root = doc.as<JsonObject>();
            if (error || root.isNull())
            {
                releaseJSONBufferLock();

                request->send(400, "application/json", F("{\"error\":json not found}")); // ERR_JSON
                return;
            }

            if (root.containsKey("utcoffset"))
            {
                utcOffsetSecs = root["utcoffset"].as<int>();
            }
            else
            {
                request->send(400, "application/json", F("{\"error\":invalid key}")); // ERR_JSON
            }
            releaseJSONBufferLock();

            doSerializeConfig = true; // serializeConfig(); //Save new settings to FS

            String resp = "{\"success\":true,\"utcoffset\":" + String(utcOffsetSecs) + "}";
            request->send(200, "application/json", resp);

            // ...
        },
        JSON_BUFFER_SIZE);
    server.addHandler(handler);
}

static char *GetTimeControlledPresets()
{
    if (!requestJSONBufferLock(23))
        return "";

    JsonArray en = doc.createNestedArray("en");
    JsonArray _timerHours = doc.createNestedArray("timerHours");
    JsonArray _timerMinutes = doc.createNestedArray("timerMinutes");
    JsonArray _timerMacro = doc.createNestedArray("timerMacro");
    JsonArray _timerWeekday = doc.createNestedArray("timerWeekday");
    JsonArray _timerMonth = doc.createNestedArray("timerMonth");
    JsonArray _timerDay = doc.createNestedArray("timerDay");
    JsonArray _timerDayEnd = doc.createNestedArray("timerDayEnd");

    for (int i = 0; i < 10; i++)
    {
        en.add(enable[i]);
        _timerHours.add(timerHours[i]);
        _timerMinutes.add(timerMinutes[i]);
        _timerMacro.add(timerMacro[i]);
        _timerWeekday.add(timerWeekday[i]);

        if (i < 8)
        {
            _timerMonth.add(timerMonth[i]);
            _timerDay.add(timerDay[i]);
            _timerDayEnd.add(timerDayEnd[i]);
        }
    }
    memset(output, 0, strlen(output));

    serializeJson(doc, output);
    releaseJSONBufferLock();
    return output;
}
static void TimeControlledPresets()
{
    server.on("/settings/time/timecontrolledpresets", HTTP_GET, [](AsyncWebServerRequest *request)
              { 
                GetUtcOffset();
                request->send(200, "text/plain", GetTimeControlledPresets()); });

    AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler(
        "/settings/time/timecontrolledpresets", [](AsyncWebServerRequest *request)
        {
            // bool verboseResponse = false;
            if (!requestJSONBufferLock(24))
                return;
            DeserializationError error = deserializeJson(doc, (uint8_t *)(request->_tempObject));
            JsonObject root = doc.as<JsonObject>();
            if (error || root.isNull())
            {
                releaseJSONBufferLock();
                request->send(400, "application/json", F("{\"error\":json not found}")); // ERR_JSON
                return;
            }
            JsonArray en = doc["en"];
            if (error || en.isNull())
            {
                releaseJSONBufferLock();

                request->send(400, "application/json", F("{\"error\": en not found}")); // ERR_JSON
                return;
            }
            JsonArray _timerHours = doc["timerHours"];
            if (error || _timerHours.isNull())
            {
                releaseJSONBufferLock();

                request->send(400, "application/json", F("{\"error\": timerHours not found}")); // ERR_JSON
                return;
            }

            JsonArray _timerMinutes = doc["timerMinutes"];
            if (error || _timerMinutes.isNull())
            {
                releaseJSONBufferLock();

                request->send(400, "application/json", F("{\"error\": timerMinutes not found}")); // ERR_JSON
                return;
            }

            JsonArray _timerMacro = doc["timerMacro"];
            if (error || _timerMacro.isNull())
            {
                releaseJSONBufferLock();

                request->send(400, "application/json", F("{\"error\": timerMacro not found}")); // ERR_JSON
                return;
            }

            JsonArray _timerWeekday = doc["timerWeekday"];
            if (error || _timerWeekday.isNull())
            {
                releaseJSONBufferLock();

                request->send(400, "application/json", F("{\"error\": timerWeekday  not found}")); // ERR_JSON
                return;
            }

            JsonArray _timerMonth = doc["timerMonth"];
            if (error || _timerMonth.isNull())
            {
                releaseJSONBufferLock();

                request->send(400, "application/json", F("{\"error\": timerMonth  not found}")); // ERR_JSON
                return;
            }

            JsonArray _timerDay = doc["timerDay"];
            if (error || _timerDay.isNull())
            {
                releaseJSONBufferLock();

                request->send(400, "application/json", F("{\"error\": timerDay  not found}")); // ERR_JSON
                return;
            }

            JsonArray _timerDayEnd = doc["timerDayEnd"];
            if (error || _timerDayEnd.isNull())
            {
                releaseJSONBufferLock();

                request->send(400, "application/json", F("{\"error\": timerDayEnd not found}")); // ERR_JSON
                return;
            }

            int i = 0;
            for (JsonVariant value : en)
            {
                enable[i] = value.as<bool>();
                i++;
            }
            i = 0;

            for (JsonVariant value : _timerHours)
            {
                timerHours[i] = value.as<int>();
                i++;
            }
            i = 0;

            for (JsonVariant value : _timerMinutes)
            {
                timerMinutes[i] = value.as<int>();
                i++;
            }
            i = 0;
            for (JsonVariant value : _timerMacro)
            {
                timerMacro[i] = value.as<int>();
                i++;
            }
            i = 0;
            for (JsonVariant value : _timerWeekday)
            {
                timerWeekday[i] = value.as<int>();
                i++;
            }
            i = 0;
            for (JsonVariant value : _timerMonth)
            {
                timerMonth[i] = value.as<int>();
                i++;
            }
            i = 0;
            for (JsonVariant value : _timerDay)
            {
                timerDay[i] = value.as<int>();
                i++;
            }
            i = 0;
            for (JsonVariant value : _timerDayEnd)
            {
                timerDayEnd[i] = value.as<int>();
                i++;
            }
            i = 0;
            releaseJSONBufferLock();

            doSerializeConfig = true; // serializeConfig(); //Save new settings to FS

            String resp = "{\"success\":true}";
            request->send(200, "application/json", resp);

            // ...
        },
        JSON_BUFFER_SIZE);
    server.addHandler(handler);
}
// Use userVar0 and userVar1 (API calls &U0=,&U1=, uint16_t)

// gets called once at boot. Do all initialization that doesn't depend on network here
void userSetup()
{
    UtcOffsetFeature();
    TimeControlledPresets();
}

// gets called every time WiFi is (re-)connected. Initialize own network interfaces here
void userConnected()
{
}

// loop. You can use "if (WLED_CONNECTED)" to check for successful connection
void userLoop()
{
}
