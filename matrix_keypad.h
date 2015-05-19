#ifndef MATRIX_KEYPAD_H_
#define MATRIX_KEYPAD_H_

/* @TODO Remove this board specific include. (If necesary, use a chip specific include.) */

#include "stm32f4xx.h"

#ifdef __cplusplus
#include <initializer_list>
#include <vector>
#include "keypad_layout.h"
#endif

#define KEYPAD_TIMER_TO_USE 12

#ifdef __cplusplus
extern "C" {
#endif

  void config_keypad_timer();

#ifdef __cplusplus

  using pin = uint16_t;         /* A bitmask. */
  using port = GPIO_TypeDef *;  /* A pointer to a struct. */
  using std::initializer_list;

  class Matrix_Keypad {
 public:
    Matrix_Keypad(initializer_list < pin > row_pins,
                  initializer_list < port > row_ports,
                  initializer_list < pin > col_pins,
                  initializer_list < port > col_ports);
    ~Matrix_Keypad();
    const static uint8_t hi_limit = 50;
    const static uint8_t lo_limit = 5;
    void time_tick_action();
    static void enable_port(port);
    void setup_row_write_col_read();
    void setup_col_write_row_read();
 private:
    void advance_row();
    void advance_col();
    void increment_sum();
    void decrement_sum();
    void init_row_write();
    void init_row_read();
    void init_col_write();
    void init_col_read();
    void write_row_read_col_time_tick_action();
    void key_press_detected_action(int index);
    void key_release_detected_action(int index);
    void VARIOUS_HACKS();
    void write_rows();
    void write_cols();
    int nrows;
    int ncols;
    int row;
    int col;
    int time_tick_counter;
    int last_press;
    int last_release;
    char *key_legend;
    pin *row_pins_;
    port *row_ports_;
    pin *col_pins_;
    port *col_ports_;
    uint8_t *debounce_sum;
    bool *key_pressed;
  };
#endif

#ifdef __cplusplus
}
#endif
#endif  // MATRIX_KEYPAD_H_
