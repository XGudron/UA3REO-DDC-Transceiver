#include <WiFi.h>
#include <WebServer.h>
#include "DeviceSettings.h"
#include "Radio.h"
#include "Panel.h"
#include "Digital.h"

WebServer server(80);
void setup() {
  Serial.begin(115200);
  const char *headers[] = {"X-Wolf-Panel", "X-Wolf-Key", "X-Wolf-Size"};
  server.collectHeaders(headers, 3);
  beginDevice(server);
  startRadio(activeSDA(), activeSCL());
  Serial.printf("YD-ESP32-S3 | Flash %lu MiB | PSRAM %lu MiB | SDA %u / SCL %u\n",
                (unsigned long)(ESP.getFlashChipSize() >> 20),
                (unsigned long)(ESP.getPsramSize() >> 20), activeSDA(), activeSCL());
  server.on("/", HTTP_GET, [] {
    server.sendHeader("Cache-Control", "no-store");
    server.sendHeader("X-Frame-Options", "DENY");
    server.send_P(200, "text/html; charset=utf-8", PANEL);
  });
  server.on("/api/state", HTTP_GET, [] {
    server.sendHeader("Cache-Control", "no-store");
    server.send(200, "application/json", radioJson());
  });
  server.on("/api/control", HTTP_POST, [] {
    if(server.header("X-Wolf-Panel") != "1"){server.send(403);return;}
    if(restartingDevice()){server.send(503, "text/plain", "Restarting");return;}
    String reply;
    uint32_t owner=strtoul(server.arg("owner").c_str(), nullptr, 10);
    int status=control(server.arg("op"),server.arg("v"),owner,reply);
    server.send(status, "text/plain; charset=utf-8",reply);
  });
  server.onNotFound([]{server.send(404);});
  beginDigital(server);
  server.begin();
}
void loop() {pollDigital();server.handleClient();serviceDevice();delay(1);}
