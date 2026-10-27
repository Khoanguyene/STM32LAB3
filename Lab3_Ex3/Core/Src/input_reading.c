#include "main.h"
#include "input_reading.h"

#define BUTTON_COUNT 3U

static GPIO_TypeDef * const button_ports[BUTTON_COUNT] = {
  BUTTON_MODE_GPIO_Port, BUTTON_INC_GPIO_Port, BUTTON_SET_GPIO_Port
};

static const uint16_t button_pins[BUTTON_COUNT] = {
  BUTTON_MODE_Pin, BUTTON_INC_Pin, BUTTON_SET_Pin
};

static GPIO_PinState last_sample[BUTTON_COUNT] = {
  GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET
};

static volatile GPIO_PinState stable_state[BUTTON_COUNT] = {
  GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET
};

void button_reading_10ms(void)
{
  for (uint8_t i = 0; i < BUTTON_COUNT; i++)
  {
    GPIO_PinState sample = HAL_GPIO_ReadPin(button_ports[i], button_pins[i]);

    if (sample == last_sample[i])
    {
      stable_state[i] = sample;
    }
    last_sample[i] = sample;
  }
}

uint8_t button_is_pressed(uint8_t index)
{
  if (index >= BUTTON_COUNT) return 0U;
  return (stable_state[index] == GPIO_PIN_RESET) ? 1U : 0U;
}
