/** ========================================================================
 *
 * @file input.hpp
 * @date 11-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

#pragma once

namespace yage::graphics
{

/* Includes ================================================================= */
/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
enum class key
{
  UNKNOWN = 0,

  space,        /* U+0020 SPACE */
  exclam,       /* U+0021 EXCLAMATION MARK */
  quotedbl,     /* U+0022 QUOTATION MARK */
  numbersign,   /* U+0023 NUMBER SIGN */
  dollar,       /* U+0024 DOLLAR SIGN */
  percent,      /* U+0025 PERCENT SIGN */
  ampersand,    /* U+0026 AMPERSAND */
  apostrophe,   /* U+0027 APOSTROPHE */
  quoteright,   /* deprecated */
  parenleft,    /* U+0028 LEFT PARENTHESIS */
  parenright,   /* U+0029 RIGHT PARENTHESIS */
  asterisk,     /* U+002A ASTERISK */
  plus,         /* U+002B PLUS SIGN */
  comma,        /* U+002C COMMA */
  minus,        /* U+002D HYPHEN-MINUS */
  period,       /* U+002E FULL STOP */
  slash,        /* U+002F SOLIDUS */
  k_0,          /* U+0030 DIGIT ZERO */
  k_1,          /* U+0031 DIGIT ONE */
  k_2,          /* U+0032 DIGIT TWO */
  k_3,          /* U+0033 DIGIT THREE */
  k_4,          /* U+0034 DIGIT FOUR */
  k_5,          /* U+0035 DIGIT FIVE */
  k_6,          /* U+0036 DIGIT SIX */
  k_7,          /* U+0037 DIGIT SEVEN */
  k_8,          /* U+0038 DIGIT EIGHT */
  k_9,          /* U+0039 DIGIT NINE */
  colon,        /* U+003A COLON */
  semicolon,    /* U+003B SEMICOLON */
  less,         /* U+003C LESS-THAN SIGN */
  equal,        /* U+003D EQUALS SIGN */
  greater,      /* U+003E GREATER-THAN SIGN */
  question,     /* U+003F QUESTION MARK */
  at,           /* U+0040 COMMERCIAL AT */
  k_A,          /* U+0041 LATIN CAPITAL LETTER A */
  k_B,          /* U+0042 LATIN CAPITAL LETTER B */
  k_C,          /* U+0043 LATIN CAPITAL LETTER C */
  k_D,          /* U+0044 LATIN CAPITAL LETTER D */
  k_E,          /* U+0045 LATIN CAPITAL LETTER E */
  k_F,          /* U+0046 LATIN CAPITAL LETTER F */
  k_G,          /* U+0047 LATIN CAPITAL LETTER G */
  k_H,          /* U+0048 LATIN CAPITAL LETTER H */
  k_I,          /* U+0049 LATIN CAPITAL LETTER I */
  k_J,          /* U+004A LATIN CAPITAL LETTER J */
  k_K,          /* U+004B LATIN CAPITAL LETTER K */
  k_L,          /* U+004C LATIN CAPITAL LETTER L */
  k_M,          /* U+004D LATIN CAPITAL LETTER M */
  k_N,          /* U+004E LATIN CAPITAL LETTER N */
  k_O,          /* U+004F LATIN CAPITAL LETTER O */
  k_P,          /* U+0050 LATIN CAPITAL LETTER P */
  k_Q,          /* U+0051 LATIN CAPITAL LETTER Q */
  k_R,          /* U+0052 LATIN CAPITAL LETTER R */
  k_S,          /* U+0053 LATIN CAPITAL LETTER S */
  k_T,          /* U+0054 LATIN CAPITAL LETTER T */
  k_U,          /* U+0055 LATIN CAPITAL LETTER U */
  k_V,          /* U+0056 LATIN CAPITAL LETTER V */
  k_W,          /* U+0057 LATIN CAPITAL LETTER W */
  k_X,          /* U+0058 LATIN CAPITAL LETTER X */
  k_Y,          /* U+0059 LATIN CAPITAL LETTER Y */
  k_Z,          /* U+005A LATIN CAPITAL LETTER Z */
  bracketleft,  /* U+005B LEFT SQUARE BRACKET */
  backslash,    /* U+005C REVERSE SOLIDUS */
  bracketright, /* U+005D RIGHT SQUARE BRACKET */
  asciicircum,  /* U+005E CIRCUMFLEX ACCENT */
  underscore,   /* U+005F LOW LINE */
  grave,        /* U+0060 GRAVE ACCENT */
  quoteleft,    /* deprecated */
  k_a,          /* U+0061 LATIN SMALL LETTER A */
  k_b,          /* U+0062 LATIN SMALL LETTER B */
  k_c,          /* U+0063 LATIN SMALL LETTER C */
  k_d,          /* U+0064 LATIN SMALL LETTER D */
  k_e,          /* U+0065 LATIN SMALL LETTER E */
  k_f,          /* U+0066 LATIN SMALL LETTER F */
  k_g,          /* U+0067 LATIN SMALL LETTER G */
  k_h,          /* U+0068 LATIN SMALL LETTER H */
  k_i,          /* U+0069 LATIN SMALL LETTER I */
  k_j,          /* U+006A LATIN SMALL LETTER J */
  k_k,          /* U+006B LATIN SMALL LETTER K */
  k_l,          /* U+006C LATIN SMALL LETTER L */
  k_m,          /* U+006D LATIN SMALL LETTER M */
  k_n,          /* U+006E LATIN SMALL LETTER N */
  k_o,          /* U+006F LATIN SMALL LETTER O */
  k_p,          /* U+0070 LATIN SMALL LETTER P */
  k_q,          /* U+0071 LATIN SMALL LETTER Q */
  k_r,          /* U+0072 LATIN SMALL LETTER R */
  k_s,          /* U+0073 LATIN SMALL LETTER S */
  k_t,          /* U+0074 LATIN SMALL LETTER T */
  k_u,          /* U+0075 LATIN SMALL LETTER U */
  k_v,          /* U+0076 LATIN SMALL LETTER V */
  k_w,          /* U+0077 LATIN SMALL LETTER W */
  k_x,          /* U+0078 LATIN SMALL LETTER X */
  k_y,          /* U+0079 LATIN SMALL LETTER Y */
  k_z,          /* U+007A LATIN SMALL LETTER Z */
  braceleft,    /* U+007B LEFT CURLY BRACKET */
  bar,          /* U+007C VERTICAL LINE */
  braceright,   /* U+007D RIGHT CURLY BRACKET */
  asciitilde,   /* U+007E TILDE */

  Home,
  Left,  /* Move left, left arrow */
  Up,    /* Move up, up arrow */
  Right, /* Move right, right arrow */
  Down,  /* Move down, down arrow */
  Prior, /* Prior, previous */
  Page_Up,
  Next, /* Next */
  Page_Down,
  End,   /* EOL */
  Begin, /* BOL */

  /* Misc functions */

  Select, /* Select, mark */
  Print,
  Execute, /* Execute, run, do */
  Insert,  /* Insert, insert here */
  Undo,
  Redo, /* Redo, again */
  Menu,
  Find,   /* Find, search */
  Cancel, /* Cancel, stop, abort, exit */
  Help,   /* Help */
  Break,
  Mode_switch,   /* Character set switch */
  script_switch, /* Alias for mode_switch */
  Num_Lock,

  /* Keypad functions, keypad numbers cleverly chosen to map to ASCII */
  KP_Space,
  KP_Tab,
  KP_Enter,

  KP_Home,
  KP_Left,
  KP_Up,
  KP_Right,
  KP_Down,
  KP_Prior,
  KP_Page_Up,
  KP_Next,
  KP_Page_Down,
  KP_End,
  KP_Begin,
  KP_Insert,
  KP_Delete,
  KP_Equal,
  KP_Multiply,
  KP_Add,
  KP_Separator,
  KP_Subtract,
  KP_Decimal,
  KP_Divide,

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
  Shift_L,
  Shift_R,
  Control_L,
  Control_R,
  Caps_Lock,
  Shift_Lock,
  Meta_L,
  Meta_R,
  Alt_L,
  Alt_R,
  Super_L,
  Super_R,
  Hyper_L,
  Hyper_R,

  MAX_KEYS,
};

enum class mouse_button
{
  UNKNOWN = 0,

  LEFT,
  RIGHT,
  MIDDLE,

  X1,
  X2,
  X3,
  X4,
  X5,
  X6,

  MAX_BUTTONS,
};

struct mouse_position
{
  int x;
  int y;
};

/* Variables ================================================================ */
/* Prototypes =============================================================== */
std::string key_to_string(key k);
key         string_to_key(const std::string & str);
char        key_to_char(key k);

std::string  mouse_button_to_string(mouse_button b);
mouse_button string_to_mouse_button(const std::string & str);

}
