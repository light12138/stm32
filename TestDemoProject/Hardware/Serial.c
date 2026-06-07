#include "Serial.h"

#include <string.h>

#define SERIAL_RX_BUFFER_SIZE 64U

extern UART_HandleTypeDef huart1;

static uint8_t rx_byte;
static volatile uint8_t rx_buffer[SERIAL_RX_BUFFER_SIZE];
static volatile uint16_t rx_head;
static volatile uint16_t rx_tail;

void Serial_Init(void)
{
  rx_head = 0;
  rx_tail = 0;
  HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
}

HAL_StatusTypeDef Serial_SendByte(uint8_t data)
{
  return HAL_UART_Transmit(&huart1, &data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef Serial_Send(const uint8_t *data, uint16_t length)
{
  if ((data == NULL) || (length == 0U))
  {
    return HAL_OK;
  }

  return HAL_UART_Transmit(&huart1, (uint8_t *)data, length, HAL_MAX_DELAY);
}

HAL_StatusTypeDef Serial_SendString(const char *string)
{
  if (string == NULL)
  {
    return HAL_OK;
  }

  return Serial_Send((const uint8_t *)string, (uint16_t)strlen(string));
}

uint8_t Serial_ReadByte(uint8_t *data)
{
  if (data == NULL)
  {
    return 0;
  }

  __disable_irq();
  if (rx_head == rx_tail)
  {
    __enable_irq();
    return 0;
  }

  *data = rx_buffer[rx_tail];
  rx_tail = (uint16_t)((rx_tail + 1U) % SERIAL_RX_BUFFER_SIZE);
  __enable_irq();
  return 1;
}

void Serial_Process(void)
{
  uint8_t data;

  while (Serial_ReadByte(&data) != 0U)
  {
    Serial_SendByte(data);
    if (data == '\r')
    {
      Serial_SendByte('\n');
    }
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    uint16_t next_head = (uint16_t)((rx_head + 1U) % SERIAL_RX_BUFFER_SIZE);

    if (next_head != rx_tail)
    {
      rx_buffer[rx_head] = rx_byte;
      rx_head = next_head;
    }

    HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
  }
}

int __io_putchar(int ch)
{
  uint8_t data = (uint8_t)ch;

  if (data == '\n')
  {
    uint8_t carriage_return = '\r';
    HAL_UART_Transmit(&huart1, &carriage_return, 1, HAL_MAX_DELAY);
  }

  HAL_UART_Transmit(&huart1, &data, 1, HAL_MAX_DELAY);
  return ch;
}
