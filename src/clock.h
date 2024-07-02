#ifndef CLOCK_H
#define CLOCK_H

void setLocalTime(uint8_t HH, uint8_t MM);
void getLocalTime(uint8_t *DD, uint8_t *HH, uint8_t *MM);
void resetDays();

#endif