#pragma once
#include <stdbool.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct { uint8_t size, bytes[96]; } WolfFrame;
typedef struct { uint32_t polls, messages, reads, delivery, dropped, shortReads; } BusStats;
bool startBus(uint8_t sda, uint8_t scl);
void setBusEnabled(bool enabled);
bool nextFrame(WolfFrame *frame);
BusStats busStats(void);
void publishCat(const char *command, uint32_t serial);
void publishMessage(const char *message);
#ifdef __cplusplus
}
#endif
