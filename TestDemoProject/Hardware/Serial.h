#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdint.h>
#include "stm32f1xx_hal.h"

void Serial_Init(void);
void Serial_Process(void);
HAL_StatusTypeDef Serial_SendByte(uint8_t data);
HAL_StatusTypeDef Serial_Send(const uint8_t *data, uint16_t length);
HAL_StatusTypeDef Serial_SendString(const char *string);
uint8_t Serial_ReadByte(uint8_t *data);

#endif
