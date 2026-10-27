#include "output_display.h"
#include "main.h"

static const uint16_t digit_pattern[10] = {
  0x3F, 0x06, 0x5B, 0x4F, 0x66,
  0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

static uint8_t digits[4] = {0, 1, 0, 0};

void output_display_set_values(uint8_t mode, uint8_t value)
{
  if (mode > 99U || value > 99U) return;

  digits[0] = mode / 10U;
  digits[1] = mode % 10U;
  digits[2] = value / 10U;
  digits[3] = value % 10U;
}

void output_display_scan(void)
{
  static uint32_t last_scan_ms = 0U;
  static uint8_t index = 0U;
  const uint16_t enable_pins[4] = {
    EN0_Pin, EN1_Pin, EN2_Pin, EN3_Pin
  };
  uint32_t now = HAL_GetTick();

  if ((uint32_t)(now - last_scan_ms) < 2U) return;
  last_scan_ms = now;

  /* Tat ca bon chu so truoc khi doi cac doan. */
  HAL_GPIO_WritePin(GPIOA,
                    EN0_Pin | EN1_Pin | EN2_Pin | EN3_Pin,
                    GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB,
                    SEG0_Pin | SEG1_Pin | SEG2_Pin | SEG3_Pin |
                    SEG4_Pin | SEG5_Pin | SEG6_Pin,
                    GPIO_PIN_SET);

  /* Common anode: muc LOW lam doan sang. */
  HAL_GPIO_WritePin(GPIOB, digit_pattern[digits[index]],
                    GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, enable_pins[index], GPIO_PIN_RESET);

  index = (index + 1U) % 4U;
}
