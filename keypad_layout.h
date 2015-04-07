#ifndef _KEYPAD_LAYOUT_H
#define _KEYPAD_LAYOUT_H

#ifndef KEYPAD_TO_USE 
#define KEYPAD_TO_USE TELEPHONE_AND_ABCD
#else
#define KEYPAD_TO_USE KEYPAD_123F_456E_789D_A0BC
#endif

#if KEYPAD_TO_USE == TELEPHONE_AND_ABCD
  constexpr char default_key_legend[] = { '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D'
  };
  /* Menu navigation special characters. */
  constexpr char scroll_up_char = default_key_legend[3];	/* 'A' */
  constexpr char select_char = default_key_legend[7];  	/* 'B' */
  constexpr char change_sign_char = select_char;
  constexpr char scroll_down_char = default_key_legend[11];	/* 'C' */
  /* Command language special characters. */
  constexpr char keypad_backspace_char = 'D';
  constexpr char keypad_return_char = '#';
  constexpr char keypad_enter_char = keypad_return_char;
  constexpr char debug_flag_char = default_key_legend[11];	/* 'D' */

#elif KEYPAD_TO_USE == KEYPAD_123F_456E_789D_A0BC
  /* I need special characters to act as backspace an return.  
   * They should be defined in a place near the keypad legend. 
   */
  constexpr char default_key_legend[] = { '1', '2', '3', 'F',
    '4', '5', '6', 'E',
    '7', '8', '9', 'D',
    'A', '0', 'B', 'C'
  };
  /* Menu navigation special characters. */
  constexpr char scroll_up_char = default_key_legend[3];	/* 'F' */
  constexpr char select_char = default_key_legend[7];	/* 'E' */
  constexpr char scroll_down_char = default_key_legend[11];	/* 'D' */
  /* Command language special characters. */
  constexpr char keypad_backspace_char = 'B';
  constexpr char keypad_return_char = 'C';
  constexpr char keypad_enter_char = keypad_return_char;
  constexpr char debug_flag_char = default_key_legend[11];	/* 'D' */
#endif


#endif   /*  _KEYPAD_LAYOUT_H  */
