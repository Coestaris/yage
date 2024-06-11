/** ========================================================================
 *
 * @file input.cpp
 * @date 11-06-2024
 * @author Maxim Kurylko <kurylko.m@ajax.systems>
 * @copyright Ajax Systems
 *
 *  ========================================================================= */

/* Includes ================================================================= */

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
#include <string>
#include <unordered_map>

#include "graphics/input.hpp"

/* Variables ================================================================ */
struct key_descriptor
{
  std::string name;
  char        code;
};

struct mouse_descriptor
{
  std::string name;
};

/* Static functions ========================================================= */
// clang-format off
static const std::unordered_map<yage::graphics::key, key_descriptor> key_descriptors = {
  { yage::graphics::key::UNKNOWN, { "UNKNOWN", 0 } },

  { yage::graphics::key::space, { "space", ' ' } },
  { yage::graphics::key::exclam, { "exclam", '!' } },
  { yage::graphics::key::quotedbl, { "quotedbl", '\"' } },
  { yage::graphics::key::numbersign, { "numbersign", '#' } },
  { yage::graphics::key::dollar, { "dollar", '$' } },
  { yage::graphics::key::percent, { "percent", '%' } },
  { yage::graphics::key::ampersand, { "ampersand", '&' } },
  { yage::graphics::key::apostrophe, { "apostrophe", '\'' } },
  { yage::graphics::key::quoteright, { "quoteright", 0 } },
  { yage::graphics::key::parenleft, { "parenleft", 0 } },
  { yage::graphics::key::parenright, { "parenright", 0 } },
  { yage::graphics::key::asterisk, { "asterisk", '*' } },
  { yage::graphics::key::plus, { "plus", '+' } },
  { yage::graphics::key::comma, { "comma", ',' } },
  { yage::graphics::key::minus, { "minus", '-' } },
  { yage::graphics::key::period, { "period", '.' } },
  { yage::graphics::key::slash, { "slash", '/' } },
  { yage::graphics::key::k_0, { "k_0", '0' } },
  { yage::graphics::key::k_1, { "k_1", '1' } },
  { yage::graphics::key::k_2, { "k_2", '2' } },
  { yage::graphics::key::k_3, { "k_3", '3' } },
  { yage::graphics::key::k_4, { "k_4", '4' } },
  { yage::graphics::key::k_5, { "k_5", '5' } },
  { yage::graphics::key::k_6, { "k_6", '6' } },
  { yage::graphics::key::k_7, { "k_7", '7' } },
  { yage::graphics::key::k_8, { "k_8", '8' } },
  { yage::graphics::key::k_9, { "k_9", '9' } },
  { yage::graphics::key::colon, { "colon", ':' } },
  { yage::graphics::key::semicolon, { "semicolon", ';' } },
  { yage::graphics::key::less, { "less", '<' } },
  { yage::graphics::key::equal, { "equal", '=' } },
  { yage::graphics::key::greater, { "greater", '>' } },
  { yage::graphics::key::question, { "question", '?' } },
  { yage::graphics::key::at, { "at", '@' } },
  { yage::graphics::key::k_A, { "k_A", 'A' } },
  { yage::graphics::key::k_B, { "k_B", 'B' } },
  { yage::graphics::key::k_C, { "k_C", 'C' } },
  { yage::graphics::key::k_D, { "k_D", 'D' } },
  { yage::graphics::key::k_E, { "k_E", 'E' } },
  { yage::graphics::key::k_F, { "k_F", 'F' } },
  { yage::graphics::key::k_G, { "k_G", 'G' } },
  { yage::graphics::key::k_H, { "k_H", 'H' } },
  { yage::graphics::key::k_I, { "k_I", 'I' } },
  { yage::graphics::key::k_J, { "k_J", 'J' } },
  { yage::graphics::key::k_K, { "k_K", 'K' } },
  { yage::graphics::key::k_L, { "k_L", 'L' } },
  { yage::graphics::key::k_M, { "k_M", 'M' } },
  { yage::graphics::key::k_N, { "k_N", 'N' } },
  { yage::graphics::key::k_O, { "k_O", 'O' } },
  { yage::graphics::key::k_P, { "k_P", 'P' } },
  { yage::graphics::key::k_Q, { "k_Q", 'Q' } },
  { yage::graphics::key::k_R, { "k_R", 'R' } },
  { yage::graphics::key::k_S, { "k_S", 'S' } },
  { yage::graphics::key::k_T, { "k_T", 'T' } },
  { yage::graphics::key::k_U, { "k_U", 'U' } },
  { yage::graphics::key::k_V, { "k_V", 'V' } },
  { yage::graphics::key::k_W, { "k_W", 'W' } },
  { yage::graphics::key::k_X, { "k_X", 'X' } },
  { yage::graphics::key::k_Y, { "k_Y", 'Y' } },
  { yage::graphics::key::k_Z, { "k_Z", 'Z' } },
  { yage::graphics::key::bracketleft, { "bracketleft", '[' } },
  { yage::graphics::key::backslash, { "backslash", '\\' } },
  { yage::graphics::key::bracketright, { "bracketright", ']' } },
  { yage::graphics::key::asciicircum, { "asciicircum", '^' } },
  { yage::graphics::key::underscore, { "underscore", '_' } },
  { yage::graphics::key::grave, { "grave", '`' } },
  { yage::graphics::key::quoteleft, { "quoteleft", 0 } },
  { yage::graphics::key::k_a, { "k_a", 'a' } },
  { yage::graphics::key::k_b, { "k_b", 'b' } },
  { yage::graphics::key::k_c, { "k_c", 'c' } },
  { yage::graphics::key::k_d, { "k_d", 'd' } },
  { yage::graphics::key::k_e, { "k_e", 'e' } },
  { yage::graphics::key::k_f, { "k_f", 'f' } },
  { yage::graphics::key::k_g, { "k_g", 'g' } },
  { yage::graphics::key::k_h, { "k_h", 'h' } },
  { yage::graphics::key::k_i, { "k_i", 'i' } },
  { yage::graphics::key::k_j, { "k_j", 'j' } },
  { yage::graphics::key::k_k, { "k_k", 'k' } },
  { yage::graphics::key::k_l, { "k_l", 'l' } },
  { yage::graphics::key::k_m, { "k_m", 'm' } },
  { yage::graphics::key::k_n, { "k_n", 'n' } },
  { yage::graphics::key::k_o, { "k_o", 'o' } },
  { yage::graphics::key::k_p, { "k_p", 'p' } },
  { yage::graphics::key::k_q, { "k_q", 'q' } },
  { yage::graphics::key::k_r, { "k_r", 'r' } },
  { yage::graphics::key::k_s, { "k_s", 's' } },
  { yage::graphics::key::k_t, { "k_t", 't' } },
  { yage::graphics::key::k_u, { "k_u", 'u' } },
  { yage::graphics::key::k_v, { "k_v", 'v' } },
  { yage::graphics::key::k_w, { "k_w", 'w' } },
  { yage::graphics::key::k_x, { "k_x", 'x' } },
  { yage::graphics::key::k_y, { "k_y", 'y' } },
  { yage::graphics::key::k_z, { "k_z", 'z' } },
  { yage::graphics::key::braceleft, { "braceleft", '{' } },
  { yage::graphics::key::bar, { "bar", '|' } },
  { yage::graphics::key::braceright, { "braceright", '}' } },
  { yage::graphics::key::asciitilde, { "asciitilde", '~' } },
  { yage::graphics::key::Home, { "Home", 0 } },
  { yage::graphics::key::Left, { "Left", 0 } },
  { yage::graphics::key::Up, { "Up", 0 } },
  { yage::graphics::key::Right, { "Right", 0 } },
  { yage::graphics::key::Down, { "Down", 0 } },
  { yage::graphics::key::Prior, { "Prior", 0 } },
  { yage::graphics::key::Page_Up, { "Page_Up", 0 } },
  { yage::graphics::key::Next, { "Next", 0 } },
  { yage::graphics::key::Page_Down, { "Page_Down", 0 } },
  { yage::graphics::key::End, { "End", 0 } },
  { yage::graphics::key::Begin, { "Begin", 0 } },
  { yage::graphics::key::Select, { "Select", 0 } },
  { yage::graphics::key::Print, { "Print", 0 } },
  { yage::graphics::key::Execute, { "Execute", 0 } },
  { yage::graphics::key::Insert, { "Insert", 0 } },
  { yage::graphics::key::Undo, { "Undo", 0 } },
  { yage::graphics::key::Redo, { "Redo", 0 } },
  { yage::graphics::key::Menu, { "Menu", 0 } },
  { yage::graphics::key::Find, { "Find", 0 } },
  { yage::graphics::key::Cancel, { "Cancel", 0 } },
  { yage::graphics::key::Help, { "Help", 0 } },
  { yage::graphics::key::Break, { "Break", 0 } },
  { yage::graphics::key::Mode_switch, { "Mode_switch", 0 } },
  { yage::graphics::key::script_switch, { "script_switch", 0 } },
  { yage::graphics::key::Num_Lock, { "Num_Lock", 0 } },
  { yage::graphics::key::KP_Space, { "KP_Space", ' ' } },
  { yage::graphics::key::KP_Tab, { "KP_Tab", 0 } },
  { yage::graphics::key::KP_Enter, { "KP_Enter", 0 } },
  { yage::graphics::key::KP_Home, { "KP_Home", 0 } },
  { yage::graphics::key::KP_Left, { "KP_Left", 0 } },
  { yage::graphics::key::KP_Up, { "KP_Up", 0 } },
  { yage::graphics::key::KP_Right, { "KP_Right", 0 } },
  { yage::graphics::key::KP_Down, { "KP_Down", 0 } },
  { yage::graphics::key::KP_Prior, { "KP_Prior", 0 } },
  { yage::graphics::key::KP_Page_Up, { "KP_Page_Up", 0 } },
  { yage::graphics::key::KP_Next, { "KP_Next", 0 } },
  { yage::graphics::key::KP_Page_Down, { "KP_Page_Down", 0 } },
  { yage::graphics::key::KP_End, { "KP_End", 0 } },
  { yage::graphics::key::KP_Begin, { "KP_Begin", 0 } },
  { yage::graphics::key::KP_Insert, { "KP_Insert", 0 } },
  { yage::graphics::key::KP_Delete, { "KP_Delete", 0 } },
  { yage::graphics::key::KP_Equal, { "KP_Equal", '=' } },
  { yage::graphics::key::KP_Multiply, { "KP_Multiply", '*' } },
  { yage::graphics::key::KP_Add, { "KP_Add", '+' } },
  { yage::graphics::key::KP_Separator, { "KP_Separator", 0 } },
  { yage::graphics::key::KP_Subtract, { "KP_Subtract", '-' } },
  { yage::graphics::key::KP_Decimal, { "KP_Decimal", '.' } },
  { yage::graphics::key::KP_Divide, { "KP_Divide", '/' } },
  { yage::graphics::key::KP_0, { "KP_0", '0' } },
  { yage::graphics::key::KP_1, { "KP_1", '1' } },
  { yage::graphics::key::KP_2, { "KP_2", '2' } },
  { yage::graphics::key::KP_3, { "KP_3", '3' } },
  { yage::graphics::key::KP_4, { "KP_4", '4' } },
  { yage::graphics::key::KP_5, { "KP_5", '5' } },
  { yage::graphics::key::KP_6, { "KP_6", '6' } },
  { yage::graphics::key::KP_7, { "KP_7", '7' } },
  { yage::graphics::key::KP_8, { "KP_8", '8' } },
  { yage::graphics::key::KP_9, { "KP_9", '9' } },
  { yage::graphics::key::F1, { "F1", 0 } },
  { yage::graphics::key::F2, { "F2", 0 } },
  { yage::graphics::key::F3, { "F3", 0 } },
  { yage::graphics::key::F4, { "F4", 0 } },
  { yage::graphics::key::F5, { "F5", 0 } },
  { yage::graphics::key::F6, { "F6", 0 } },
  { yage::graphics::key::F7, { "F7", 0 } },
  { yage::graphics::key::F8, { "F8", 0 } },
  { yage::graphics::key::F9, { "F9", 0 } },
  { yage::graphics::key::F10, { "F10", 0 } },
  { yage::graphics::key::F11, { "F11", 0 } },
  { yage::graphics::key::F12, { "F12", 0 } },
  { yage::graphics::key::Shift_L, { "Shift_L", 0 } },
  { yage::graphics::key::Shift_R, { "Shift_R", 0 } },
  { yage::graphics::key::Control_L, { "Control_L", 0 } },
  { yage::graphics::key::Control_R, { "Control_R", 0 } },
  { yage::graphics::key::Caps_Lock, { "Caps_Lock", 0 } },
  { yage::graphics::key::Shift_Lock, { "Shift_Lock", 0 } },
  { yage::graphics::key::Meta_L, { "Meta_L", 0 } },
  { yage::graphics::key::Meta_R, { "Meta_R", 0 } },
  { yage::graphics::key::Alt_L, { "Alt_L", 0 } },
  { yage::graphics::key::Alt_R, { "Alt_R", 0 } },
  { yage::graphics::key::Super_L, { "Super_L", 0 } },
  { yage::graphics::key::Super_R, { "Super_R", 0 } },
  { yage::graphics::key::Hyper_L, { "Hyper_L", 0 } },
  { yage::graphics::key::Hyper_R, { "Hyper_R", 0 } },
};
// clang-format on

// clang-format off
static const std::unordered_map<yage::graphics::mouse_button, mouse_descriptor> mouse_descriptors = {
  { yage::graphics::mouse_button::UNKNOWN, { "UNKNOWN" } },

  { yage::graphics::mouse_button::LEFT, { "LEFT" } },
  { yage::graphics::mouse_button::MIDDLE, { "MIDDLE" } },
  { yage::graphics::mouse_button::RIGHT, { "RIGHT" } },

  { yage::graphics::mouse_button::X1, { "X1" } },
  { yage::graphics::mouse_button::X2, { "X2" } },
  { yage::graphics::mouse_button::X3, { "X3" } },
  { yage::graphics::mouse_button::X4, { "X4" } },
  { yage::graphics::mouse_button::X5, { "X5" } },
  { yage::graphics::mouse_button::X6, { "X6" } },
};
// clang-format on

/* Shared functions ========================================================= */
std::string yage::graphics::key_to_string(yage::graphics::key k)
{
  auto it = key_descriptors.find(k);
  if (it == key_descriptors.end())
  {
    return "UNKNOWN";
  }

  return it->second.name;
}

yage::graphics::key yage::graphics::string_to_key(const std::string & str)
{
  for (const auto & [key, descriptor] : key_descriptors)
  {
    if (descriptor.name == str)
    {
      return key;
    }
  }

  return yage::graphics::key::UNKNOWN;
}

char yage::graphics::key_to_char(yage::graphics::key k)
{
  auto it = key_descriptors.find(k);
  if (it == key_descriptors.end())
  {
    return 0;
  }

  return it->second.code;
}

std::string
yage::graphics::mouse_button_to_string(yage::graphics::mouse_button b)
{
  auto it = mouse_descriptors.find(b);
  if (it == mouse_descriptors.end())
  {
    return "UNKNOWN";
  }

  return it->second.name;
}

yage::graphics::mouse_button
yage::graphics::string_to_mouse_button(const std::string & str)
{
  for (const auto & [button, descriptor] : mouse_descriptors)
  {
    if (descriptor.name == str)
    {
      return button;
    }
  }

  return yage::graphics::mouse_button::UNKNOWN;
}
