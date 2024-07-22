/** ========================================================================
 *
 * @file keyboard.hpp
 * @date 11-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

/* Includes ================================================================= */

namespace yc
{
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
enum class key
{
  UNKNOWN = 0,

  /* Control keys */
  ENTER,     /* U+000A LINE FEED */
  DEL,       /* U+007F DELETE */
  BACKSPACE, /* U+0008 BACKSPACE */
  ESCAPE,    /* U+001B ESCAPE */
  TAB,       /* U+0009 CHARACTER TABULATION */
  SPACE,     /* U+0020 SPACE */
  SCROLL_LOCK,
  PAUSE,
  SYS_REQ,
  HOME,
  LEFT,  /* Move left, left arrow */
  UP,    /* Move up, up arrow */
  RIGHT, /* Move right, right arrow */
  DOWN,  /* Move down, down arrow */
  PRIOR, /* Prior, previous */
  PAGE_UP,
  NEXT, /* Next */
  PAGE_DOWN,
  END,   /* EOL */
  BEGIN, /* BOL */

  /* Special keys */
  SK1,  /* Usually: semicolon, colon */
  SK2,  /* Usually: slash, question mark, dot */
  SK3,  /* Usually: backsplash, vertical bar */
  SK4,  /* Usually: left square bracket, left brace */
  SK5,  /* Usually: right square bracket, right brace */
  SK6,  /* Usually: single quote, double quote */
  SK7,  /* Usually: backtick, tilde */
  SK8,  /* Usually: comma, less than */
  SK9,  /* Usually: dot, greater than */
  SK10, /* Usually: minus, underscore */
  SK11, /* Usually: equal, plus */

  /* Letters */
  A, /* U+0041 LATIN CAPITAL LETTER A */
  B, /* U+0042 LATIN CAPITAL LETTER B */
  C, /* U+0043 LATIN CAPITAL LETTER C */
  D, /* U+0044 LATIN CAPITAL LETTER D */
  E, /* U+0045 LATIN CAPITAL LETTER E */
  F, /* U+0046 LATIN CAPITAL LETTER F */
  G, /* U+0047 LATIN CAPITAL LETTER G */
  H, /* U+0048 LATIN CAPITAL LETTER H */
  I, /* U+0049 LATIN CAPITAL LETTER I */
  J, /* U+004A LATIN CAPITAL LETTER J */
  K, /* U+004B LATIN CAPITAL LETTER K */
  L, /* U+004C LATIN CAPITAL LETTER L */
  M, /* U+004D LATIN CAPITAL LETTER M */
  N, /* U+004E LATIN CAPITAL LETTER N */
  O, /* U+004F LATIN CAPITAL LETTER O */
  P, /* U+0050 LATIN CAPITAL LETTER P */
  Q, /* U+0051 LATIN CAPITAL LETTER Q */
  R, /* U+0052 LATIN CAPITAL LETTER R */
  S, /* U+0053 LATIN CAPITAL LETTER S */
  T, /* U+0054 LATIN CAPITAL LETTER T */
  U, /* U+0055 LATIN CAPITAL LETTER U */
  V, /* U+0056 LATIN CAPITAL LETTER V */
  W, /* U+0057 LATIN CAPITAL LETTER W */
  X, /* U+0058 LATIN CAPITAL LETTER X */
  Y, /* U+0059 LATIN CAPITAL LETTER Y */
  Z, /* U+005A LATIN CAPITAL LETTER Z */

  /* Numbers */
  K0, /* U+0030 DIGIT ZERO */
  K1, /* U+0031 DIGIT ONE */
  K2, /* U+0032 DIGIT TWO */
  K3, /* U+0033 DIGIT THREE */
  K4, /* U+0034 DIGIT FOUR */
  K5, /* U+0035 DIGIT FIVE */
  K6, /* U+0036 DIGIT SIX */
  K7, /* U+0037 DIGIT SEVEN */
  K8, /* U+0038 DIGIT EIGHT */
  K9, /* U+0039 DIGIT NINE */

  /* Misc functions */
  SELECT, /* Select, mark */
  PRINT,
  EXECUTE, /* Execute, run, do */
  INSERT,  /* Insert, insert here */
  UNDO,
  REDO, /* Redo, again */
  MENU,
  FIND,   /* Find, search */
  CANCEL, /* Cancel, stop, abort, exit */
  HELP,   /* Help */
  BREAK,
  CLEAR,
  NUM_LOCK,

  /* Keypad functions */
  KP_SPACE,
  KP_TAB,
  KP_ENTER,
  KP_HOME,
  KP_LEFT,
  KP_UP,
  KP_RIGHT,
  KP_DOWN,
  KP_PRIOR,
  KP_PAGE_UP,
  KP_NEXT,
  KP_PAGE_DOWN,
  KP_END,
  KP_BEGIN,
  KP_INSERT,
  KP_DELETE,
  KP_EQUAL,
  KP_MULTIPLY,
  KP_ADD,
  KP_SEPARATOR,
  KP_SUBTRACT,
  KP_DECIMAL,
  KP_DIVIDE,
  KP_0,
  KP_1,
  KP_2,
  KP_3,
  KP_4,
  KP_5,
  KP_6,
  KP_7,
  KP_8,
  KP_9,

  /* Function keys */
  F1,
  F2,
  F3,
  F4,
  F5,
  F6,
  F7,
  F8,
  F9,
  F10,
  F11,
  F12,

  /* Key state modifier keys */
  SHIFT_L,
  SHIFT_R,
  CONTROL_L,
  CONTROL_R,
  CAPS_LOCK,
  SHIFT_LOCK,
  META_L,
  META_R,
  ALT_L,
  ALT_R,
  SUPER_L,
  SUPER_R,
  HYPER_L,
  HYPER_R,

  /* Reserved */
  OEM_X1,
  OEM_X2,
  OEM_X3,
  OEM_X4,
  OEM_X5,
  OEM_X6,
  OEM_X7,
  OEM_X8,
  OEM_X9,
  OEM_X11,
  OEM_X12,
  OEM_X13,
  OEM_X14,
  OEM_X15,
  OEM_X16,
  OEM_X17,
  OEM_X18,
  OEM_X19,

  MAX_KEYS,
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
std::string to_string(key k);
char        key_to_char(key k);
key         key_from_string(const std::string & str);

}
