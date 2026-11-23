#pragma once
#include <WebServer.h>
struct SkimmerConfig {
  char host[64];
  char login[24];
  uint16_t port;
};
void startPanServices(WebServer &s, const SkimmerConfig &config);
String resourceJson();
