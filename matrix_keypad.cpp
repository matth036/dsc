#include "matrix_keypad.h"
/*
  #include "main.h" makes this class less portable.
  I think it was done this way because 
  void main_keypad_time_tick_action()
  is in main.cpp.
 */
#include "main.h"
#include "controller.h"

#if KEYPAD_TIMER_TO_USE ==  12
void config_keypad_timer()
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
  NVIC_InitTypeDef NVIC_InitStructure;
  /* Enable the TIM12 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM8_BRK_TIM12_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  /* Count at 1MHz
   * TIM12 runs at 42MHz.  (Max interface clock in chip documentation)
   * (Counter frequency) = (Clock frequncy)/(Prescaler + 1)
   */
  uint16_t TIM12_PrescalerValue = 41;	// 1MHz clock.
  TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
  TIM_TimeBaseStructure.TIM_Prescaler = TIM12_PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);

  TIM_PrescalerConfig(TIM12, TIM12_PrescalerValue, TIM_PSCReloadMode_Immediate);
  TIM_OCInitTypeDef TIM_OCInitStructure;
  /* Output Compare Timing Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  /** @TODO find out what the hell this does. (Previously I mindlessly left the TIM3_CCR1_Val in place.) */
  TIM_OCInitStructure.TIM_Pulse = 2500;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(TIM12, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM12, TIM_OCPreload_Disable);

  TIM_ITConfig(TIM12, TIM_IT_CC1, ENABLE);

  TIM_Cmd(TIM12, ENABLE);
}
#endif

/* 
   This is the action called by the keypad timer.
   It may be neccesary to move this to main.cpp.
*/

void Matrix_Keypad::time_tick_action()
{
  ++time_tick_counter;
  write_row_read_col_time_tick_action();
}

Matrix_Keypad::Matrix_Keypad(initializer_list < pin > row_pins,
			     initializer_list < port > row_ports,
			     initializer_list < pin > col_pins,
			     initializer_list < port > col_ports)
{

  for (auto p = row_ports.begin(); p != row_ports.end(); ++p) {
    enable_port(*p);
  }
  for (auto p = col_ports.begin(); p != col_ports.end(); ++p) {
    enable_port(*p);
  }
  nrows = row_pins.size();
  ncols = col_pins.size();
  row = 0;
  col = 0;

  row_pins_ = new pin[nrows];
  row_ports_ = new port[nrows];
  int i = 0;
  for (auto it = row_pins.begin(); it != row_pins.end(); ++it) {
    row_pins_[i] = *it;
    ++i;
  }
  i = 0;
  for (auto it = row_ports.begin(); it != row_ports.end(); ++it) {
    row_ports_[i] = *it;
    ++i;
  }
  while (i < nrows) {		/* Use the last port listed for other ports in the row. */
    row_ports_[i] = row_ports_[i - 1];
    ++i;
  }
  col_pins_ = new pin[ncols];
  col_ports_ = new port[ncols];
  i = 0;
  for (auto it = col_pins.begin(); it != col_pins.end(); ++it) {
    col_pins_[i] = *it;
    ++i;
  }
  i = 0;
  for (auto it = col_ports.begin(); it != col_ports.end(); ++it) {
    col_ports_[i] = *it;
    ++i;
  }
  while (i < ncols) {		/* Use the last port listed for other ports in the col. */
    col_ports_[i] = col_ports_[i - 1];
    ++i;
  }
  debounce_sum = new uint8_t[nrows * ncols];
  key_pressed = new bool[nrows * ncols];
  for (i = 0; i < nrows * ncols; ++i) {
    debounce_sum[i] = 0;
    key_pressed[i] = false;
  }
  key_legend = new char[nrows * ncols];
  for (i = 0; i < nrows * ncols; ++i) {
    key_legend[i] = default_key_legend[i % 16];
  }
  setup_row_write_col_read();
  time_tick_counter = 0;
}

Matrix_Keypad::~Matrix_Keypad()
{
  delete[]key_pressed;
  delete[]debounce_sum;
  delete[]col_ports_;
  delete[]col_pins_;
  delete[]row_ports_;
  delete[]row_pins_;
  delete[]key_legend;
}

void Matrix_Keypad::advance_row()
{
  int temp = (row + 1) % nrows;
  row = temp;
}

void Matrix_Keypad::advance_col()
{
  int temp = (col + 1) % ncols;
  col = temp;
}

void Matrix_Keypad::increment_sum()
{
  int index = row * ncols + col;
  if (debounce_sum[index] < Matrix_Keypad::hi_limit) {
    ++debounce_sum[index];
    if (debounce_sum[index] == Matrix_Keypad::hi_limit && !key_pressed[index]) {
      /* Transition from unpressed state to pressed state. */
      key_pressed[index] = true;
      key_press_detected_action(index);
    }
  }
}

void Matrix_Keypad::decrement_sum()
{
  int index = row * ncols + col;
  if (debounce_sum[index] > 0) {
    --debounce_sum[index];
    if (debounce_sum[index] == Matrix_Keypad::lo_limit && key_pressed[index]) {
      /* Transition from pressed state to unpressed state. */
      key_pressed[index] = false;
      key_release_detected_action(index);
    }
  }
}

void Matrix_Keypad::write_row_read_col_time_tick_action()
{
  bool read_values[ncols];
  uint16_t data;
  for (int c = 0; c < ncols; ++c) {
    data = GPIO_ReadInputData(col_ports_[c]);
    read_values[c] = data & col_pins_[c];
  }
  for (int c = 0; c < ncols; ++c) {
    col = c;			/* One of the implicit arguments of increment_sum() and decrement_sum() */
    if (read_values[c]) {
      increment_sum();
    } else {
      decrement_sum();
    }
  }
  advance_row();
  write_rows();
}

void Matrix_Keypad::key_press_detected_action(int index)
{
  last_press = index;
  dsc_controller::put_char(key_legend[index]);
}

void Matrix_Keypad::key_release_detected_action(int index)
{
  last_release = index;
}

void Matrix_Keypad::VARIOUS_HACKS()
{

}

// GPIO_ResetBits(KEYPAD_WRITE_PORT, KEYPAD_WRITE_PIN_0 );
//  GPIO_SetBits(KEYPAD_WRITE_PORT, KEYPAD_WRITE_PIN_1 ); 
void Matrix_Keypad::write_rows()
{
  for (int r = 0; r < nrows; ++r) {
    GPIO_ResetBits(row_ports_[r], row_pins_[r]);
  }
  GPIO_SetBits(row_ports_[row], row_pins_[row]);
}

void Matrix_Keypad::write_cols()
{
  for (int c = 0; c < ncols; ++c) {
    GPIO_ResetBits(col_ports_[c], col_pins_[c]);
  }
  GPIO_SetBits(col_ports_[col], col_pins_[col]);
}

/* This static method should not be a member function. */
void Matrix_Keypad::enable_port(port p)
{
  if (p == GPIOA) {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  } else if (p == GPIOB) {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  } else if (p == GPIOC) {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  } else if (p == GPIOD) {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  } else if (p == GPIOE) {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  } else if (p == GPIOF) {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  } else if (p == GPIOG) {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
  } else if (p == GPIOH) {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
  } else if (p == GPIOI) {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);
  } else {
    assert_param(0);
  }
}

void Matrix_Keypad::setup_row_write_col_read()
{
  init_row_write();
  init_col_read();
  row = 0;
  col = 0;
  write_rows();
}

void Matrix_Keypad::setup_col_write_row_read()
{
  init_col_write();
  init_row_read();
  row = 0;
  col = 0;
  write_cols();
}

void Matrix_Keypad::init_row_write()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  for (int r = 0; r < nrows; ++r) {
    GPIO_InitStructure.GPIO_Pin = row_pins_[r];
    GPIO_Init(row_ports_[r], &GPIO_InitStructure);
  }
}

void Matrix_Keypad::init_row_read()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  for (int r = 0; r < nrows; ++r) {
    GPIO_InitStructure.GPIO_Pin = row_pins_[r];
    GPIO_Init(row_ports_[r], &GPIO_InitStructure);
  }
}

void Matrix_Keypad::init_col_write()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  for (int c = 0; c < ncols; ++c) {
    GPIO_InitStructure.GPIO_Pin = col_pins_[c];
    GPIO_Init(row_ports_[c], &GPIO_InitStructure);
  }
}

void Matrix_Keypad::init_col_read()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  for (int c = 0; c < ncols; ++c) {
    GPIO_InitStructure.GPIO_Pin = col_pins_[c];
    GPIO_Init(col_ports_[c], &GPIO_InitStructure);
  }
}
