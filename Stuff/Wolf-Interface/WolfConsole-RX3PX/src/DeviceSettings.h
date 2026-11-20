#pragma once
#include <WebServer.h>

void beginDevice(WebServer &server);
void serviceDevice();
uint8_t activeSDA();
uint8_t activeSCL();
bool restartingDevice();
