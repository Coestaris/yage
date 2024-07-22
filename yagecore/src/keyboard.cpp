/** ========================================================================
 *
 * @file keyboard.cpp
 * @date 11-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
#include <string>
#include <unordered_map>

#include "yagecore/keyboard.hpp"

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
static const std::unordered_map<yc::key, key_descriptor> key_descriptors = {
  { yc::key::UNKNOWN, { "UNKNOWN", 0 } },

  /* Control keys */
  { yc::key::ENTER, { "ENTER", 0 } },
  { yc::key::DEL, { "DEL", 0 } },
  { yc::key::BACKSPACE, { "BACKSPACE", 0 } },
  { yc::key::ESCAPE, { "ESCAPE", 0 } },
  { yc::key::TAB, { "TAB", 0 } },
  { yc::key::SPACE, { "SPACE", 0 } },
  { yc::key::SCROLL_LOCK, { "SCROLL_LOCK", 0 } },
  { yc::key::PAUSE, { "PAUSE", 0 } },
  { yc::key::SYS_REQ, { "SYS_REQ", 0 } },
  { yc::key::HOME, { "HOME", 0 } },
  { yc::key::LEFT, { "LEFT", 0 } },
  { yc::key::UP, { "UP", 0 } },
  { yc::key::RIGHT, { "RIGHT", 0 } },
  { yc::key::DOWN, { "DOWN", 0 } },
  { yc::key::PRIOR, { "PRIOR", 0 } },
  { yc::key::PAGE_UP, { "PAGE_UP", 0 } },
  { yc::key::NEXT, { "NEXT", 0 } },
  { yc::key::PAGE_DOWN, { "PAGE_DOWN", 0 } },
  { yc::key::END, { "END", 0 } },
  { yc::key::BEGIN, { "BEGIN", 0 } },

  /* Special keys */
  { yc::key::SK1, { "SK1", 0 } },
  { yc::key::SK2, { "SK2", 0 } },
  { yc::key::SK3, { "SK3", 0 } },
  { yc::key::SK4, { "SK4", 0 } },
  { yc::key::SK5, { "SK5", 0 } },
  { yc::key::SK6, { "SK6", 0 } },
  { yc::key::SK7, { "SK7", 0 } },
  { yc::key::SK8, { "SK8", 0 } },
  { yc::key::SK9, { "SK9", 0 } },
  { yc::key::SK10, { "SK10", 0 } },
  { yc::key::SK11, { "SK11", 0 } },

  /* Letters */
  { yc::key::A, { "A", 'A' } },
  { yc::key::B, { "B", 'B' } },
  { yc::key::C, { "C", 'C' } },
  { yc::key::D, { "D", 'D' } },
  { yc::key::E, { "E", 'E' } },
  { yc::key::F, { "F", 'F' } },
  { yc::key::G, { "G", 'G' } },
  { yc::key::H, { "H", 'H' } },
  { yc::key::I, { "I", 'I' } },
  { yc::key::J, { "J", 'J' } },
  { yc::key::K, { "K", 'K' } },
  { yc::key::L, { "L", 'L' } },
  { yc::key::M, { "M", 'M' } },
  { yc::key::N, { "N", 'N' } },
  { yc::key::O, { "O", 'O' } },
  { yc::key::P, { "P", 'P' } },
  { yc::key::Q, { "Q", 'Q' } },
  { yc::key::R, { "R", 'R' } },
  { yc::key::S, { "S", 'S' } },
  { yc::key::T, { "T", 'T' } },
  { yc::key::U, { "U", 'U' } },
  { yc::key::V, { "V", 'V' } },
  { yc::key::W, { "W", 'W' } },
  { yc::key::X, { "X", 'X' } },
  { yc::key::Y, { "Y", 'Y' } },
  { yc::key::Z, { "Z", 'Z' } },

  { yc::key::K0, { "0", '0' } },
  { yc::key::K1, { "1", '1' } },
  { yc::key::K2, { "2", '2' } },
  { yc::key::K3, { "3", '3' } },
  { yc::key::K4, { "4", '4' } },
  { yc::key::K5, { "5", '5' } },
  { yc::key::K6, { "6", '6' } },
  { yc::key::K7, { "7", '7' } },
  { yc::key::K8, { "8", '8' } },
  { yc::key::K9, { "9", '9' } },

  /* Misc functions */
  { yc::key::SELECT, { "SELECT", 0 } },
  { yc::key::PRINT, { "PRINT", 0 } },
  { yc::key::EXECUTE, { "EXECUTE", 0 } },
  { yc::key::INSERT, { "INSERT", 0 } },
  { yc::key::UNDO, { "UNDO", 0 } },
  { yc::key::REDO, { "REDO", 0 } },
  { yc::key::MENU, { "MENU", 0 } },
  { yc::key::FIND, { "FIND", 0 } },
  { yc::key::CANCEL, { "CANCEL", 0 } },
  { yc::key::HELP, { "HELP", 0 } },
  { yc::key::BREAK, { "BREAK", 0 } },
  { yc::key::CLEAR, { "CLEAR", 0 } },
  { yc::key::NUM_LOCK, { "NUM_LOCK", 0 } },

  /* Keypad functions */
  { yc::key::KP_SPACE, { "KP_SPACE", ' ' } },
  { yc::key::KP_TAB, { "KP_TAB", '\t' } },
  { yc::key::KP_ENTER, { "KP_ENTER", 0 } },
  { yc::key::KP_HOME, { "KP_HOME", 0 } },
  { yc::key::KP_LEFT, { "KP_LEFT", 0 } },
  { yc::key::KP_UP, { "KP_UP", 0 } },
  { yc::key::KP_RIGHT, { "KP_RIGHT", 0 } },
  { yc::key::KP_DOWN, { "KP_DOWN", 0 } },
  { yc::key::KP_PRIOR, { "KP_PRIOR", 0 } },
  { yc::key::KP_PAGE_UP, { "KP_PAGE_UP", 0 } },
  { yc::key::KP_NEXT, { "KP_NEXT", 0 } },
  { yc::key::KP_PAGE_DOWN, { "KP_PAGE_DOWN", 0 } },
  { yc::key::KP_END, { "KP_END", 0 } },
  { yc::key::KP_BEGIN, { "KP_BEGIN", 0 } },
  { yc::key::KP_INSERT, { "KP_INSERT", 0 } },
  { yc::key::KP_DELETE, { "KP_DELETE", 0 } },
  { yc::key::KP_EQUAL, { "KP_EQUAL", '=' } },
  { yc::key::KP_MULTIPLY, { "KP_MULTIPLY", '*' } },
  { yc::key::KP_ADD, { "KP_ADD", '+' } },
  { yc::key::KP_SEPARATOR, { "KP_SEPARATOR", 0 } },
  { yc::key::KP_SUBTRACT, { "KP_SUBTRACT", '-' } },
  { yc::key::KP_DECIMAL, { "KP_DECIMAL", '.' } },
  { yc::key::KP_DIVIDE, { "KP_DIVIDE", '/' } },
  { yc::key::KP_0, { "KP_0", '0' } },
  { yc::key::KP_1, { "KP_1", '1' } },
  { yc::key::KP_2, { "KP_2", '2' } },
  { yc::key::KP_3, { "KP_3", '3' } },
  { yc::key::KP_4, { "KP_4", '4' } },
  { yc::key::KP_5, { "KP_5", '5' } },
  { yc::key::KP_6, { "KP_6", '6' } },
  { yc::key::KP_7, { "KP_7", '7' } },
  { yc::key::KP_8, { "KP_8", '8' } },
  { yc::key::KP_9, { "KP_9", '9' } },

  /* Function keys */
  { yc::key::F1, { "F1", 0 } },
  { yc::key::F2, { "F2", 0 } },
  { yc::key::F3, { "F3", 0 } },
  { yc::key::F4, { "F4", 0 } },
  { yc::key::F5, { "F5", 0 } },
  { yc::key::F6, { "F6", 0 } },
  { yc::key::F7, { "F7", 0 } },
  { yc::key::F8, { "F8", 0 } },
  { yc::key::F9, { "F9", 0 } },
  { yc::key::F10, { "F10", 0 } },
  { yc::key::F11, { "F11", 0 } },
  { yc::key::F12, { "F12", 0 } },

  /* Key state modifier keys */
  { yc::key::SHIFT_L, { "SHIFT_L", 0 } },
  { yc::key::SHIFT_R, { "SHIFT_R", 0 } },
  { yc::key::CONTROL_L, { "CONTROL_L", 0 } },
  { yc::key::CONTROL_R, { "CONTROL_R", 0 } },
  { yc::key::CAPS_LOCK, { "CAPS_LOCK", 0 } },
  { yc::key::SHIFT_LOCK, { "SHIFT_LOCK", 0 } },
  { yc::key::META_L, { "META_L", 0 } },
  { yc::key::META_R, { "META_R", 0 } },
  { yc::key::ALT_L, { "ALT_L", 0 } },
  { yc::key::ALT_R, { "ALT_R", 0 } },
  { yc::key::SUPER_L, { "SUPER_L", 0 } },
  { yc::key::SUPER_R, { "SUPER_R", 0 } },
  { yc::key::HYPER_L, { "HYPER_L", 0 } },
  { yc::key::HYPER_R, { "HYPER_R", 0 } },

  /* Reserved */
  { yc::key::OEM_X1, { "OEM_X1", 0 } },
  { yc::key::OEM_X2, { "OEM_X2", 0 } },
  { yc::key::OEM_X3, { "OEM_X3", 0 } },
  { yc::key::OEM_X4, { "OEM_X4", 0 } },
  { yc::key::OEM_X5, { "OEM_X5", 0 } },
  { yc::key::OEM_X6, { "OEM_X6", 0 } },
  { yc::key::OEM_X7, { "OEM_X7", 0 } },
  { yc::key::OEM_X8, { "OEM_X8", 0 } },
  { yc::key::OEM_X9, { "OEM_X9", 0 } },
  { yc::key::OEM_X11, { "OEM_X11", 0 } },
  { yc::key::OEM_X12, { "OEM_X12", 0 } },
  { yc::key::OEM_X13, { "OEM_X13", 0 } },
  { yc::key::OEM_X14, { "OEM_X14", 0 } },
  { yc::key::OEM_X15, { "OEM_X15", 0 } },
  { yc::key::OEM_X16, { "OEM_X16", 0 } },
  { yc::key::OEM_X17, { "OEM_X17", 0 } },
  { yc::key::OEM_X18, { "OEM_X18", 0 } },
  { yc::key::OEM_X19, { "OEM_X19", 0 } },
};

/* Shared functions ========================================================= */
std::string yc::to_string(yc::key k)
{
  auto it = key_descriptors.find(k);
  if (it == key_descriptors.end())
  {
    return "UNKNOWN";
  }

  return it->second.name;
}

char yc::key_to_char(yc::key k)
{
  auto it = key_descriptors.find(k);
  if (it == key_descriptors.end())
  {
    return 0;
  }

  return it->second.code;
}

yc::key yc::key_from_string(const std::string & str)
{
  for (const auto & [key, descriptor] : key_descriptors)
  {
    if (descriptor.name == str)
    {
      return key;
    }
  }

  return yc::key::UNKNOWN;
}
