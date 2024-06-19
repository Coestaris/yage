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

#include "keyboard.hpp"

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
static const std::unordered_map<yage::core::key, key_descriptor> key_descriptors
      = {
          { yage::core::key::UNKNOWN, { "UNKNOWN", 0 } },

          /* Control keys */
          { yage::core::key::ENTER, { "ENTER", 0 } },
          { yage::core::key::DEL, { "DEL", 0 } },
          { yage::core::key::BACKSPACE, { "BACKSPACE", 0 } },
          { yage::core::key::ESCAPE, { "ESCAPE", 0 } },
          { yage::core::key::TAB, { "TAB", 0 } },
          { yage::core::key::SPACE, { "SPACE", 0 } },
          { yage::core::key::SCROLL_LOCK, { "SCROLL_LOCK", 0 } },
          { yage::core::key::PAUSE, { "PAUSE", 0 } },
          { yage::core::key::SYS_REQ, { "SYS_REQ", 0 } },
          { yage::core::key::HOME, { "HOME", 0 } },
          { yage::core::key::LEFT, { "LEFT", 0 } },
          { yage::core::key::UP, { "UP", 0 } },
          { yage::core::key::RIGHT, { "RIGHT", 0 } },
          { yage::core::key::DOWN, { "DOWN", 0 } },
          { yage::core::key::PRIOR, { "PRIOR", 0 } },
          { yage::core::key::PAGE_UP, { "PAGE_UP", 0 } },
          { yage::core::key::NEXT, { "NEXT", 0 } },
          { yage::core::key::PAGE_DOWN, { "PAGE_DOWN", 0 } },
          { yage::core::key::END, { "END", 0 } },
          { yage::core::key::BEGIN, { "BEGIN", 0 } },

          /* Special keys */
          { yage::core::key::SK1, { "SK1", 0 } },
          { yage::core::key::SK2, { "SK2", 0 } },
          { yage::core::key::SK3, { "SK3", 0 } },
          { yage::core::key::SK4, { "SK4", 0 } },
          { yage::core::key::SK5, { "SK5", 0 } },
          { yage::core::key::SK6, { "SK6", 0 } },
          { yage::core::key::SK7, { "SK7", 0 } },
          { yage::core::key::SK8, { "SK8", 0 } },
          { yage::core::key::SK9, { "SK9", 0 } },
          { yage::core::key::SK10, { "SK10", 0 } },
          { yage::core::key::SK11, { "SK11", 0 } },

          /* Letters */
          { yage::core::key::A, { "A", 'A' } },
          { yage::core::key::B, { "B", 'B' } },
          { yage::core::key::C, { "C", 'C' } },
          { yage::core::key::D, { "D", 'D' } },
          { yage::core::key::E, { "E", 'E' } },
          { yage::core::key::F, { "F", 'F' } },
          { yage::core::key::G, { "G", 'G' } },
          { yage::core::key::H, { "H", 'H' } },
          { yage::core::key::I, { "I", 'I' } },
          { yage::core::key::J, { "J", 'J' } },
          { yage::core::key::K, { "K", 'K' } },
          { yage::core::key::L, { "L", 'L' } },
          { yage::core::key::M, { "M", 'M' } },
          { yage::core::key::N, { "N", 'N' } },
          { yage::core::key::O, { "O", 'O' } },
          { yage::core::key::P, { "P", 'P' } },
          { yage::core::key::Q, { "Q", 'Q' } },
          { yage::core::key::R, { "R", 'R' } },
          { yage::core::key::S, { "S", 'S' } },
          { yage::core::key::T, { "T", 'T' } },
          { yage::core::key::U, { "U", 'U' } },
          { yage::core::key::V, { "V", 'V' } },
          { yage::core::key::W, { "W", 'W' } },
          { yage::core::key::X, { "X", 'X' } },
          { yage::core::key::Y, { "Y", 'Y' } },
          { yage::core::key::Z, { "Z", 'Z' } },

          { yage::core::key::K0, { "0", '0' } },
          { yage::core::key::K1, { "1", '1' } },
          { yage::core::key::K2, { "2", '2' } },
          { yage::core::key::K3, { "3", '3' } },
          { yage::core::key::K4, { "4", '4' } },
          { yage::core::key::K5, { "5", '5' } },
          { yage::core::key::K6, { "6", '6' } },
          { yage::core::key::K7, { "7", '7' } },
          { yage::core::key::K8, { "8", '8' } },
          { yage::core::key::K9, { "9", '9' } },

          /* Misc functions */
          { yage::core::key::SELECT, { "SELECT", 0 } },
          { yage::core::key::PRINT, { "PRINT", 0 } },
          { yage::core::key::EXECUTE, { "EXECUTE", 0 } },
          { yage::core::key::INSERT, { "INSERT", 0 } },
          { yage::core::key::UNDO, { "UNDO", 0 } },
          { yage::core::key::REDO, { "REDO", 0 } },
          { yage::core::key::MENU, { "MENU", 0 } },
          { yage::core::key::FIND, { "FIND", 0 } },
          { yage::core::key::CANCEL, { "CANCEL", 0 } },
          { yage::core::key::HELP, { "HELP", 0 } },
          { yage::core::key::BREAK, { "BREAK", 0 } },
          { yage::core::key::CLEAR, { "CLEAR", 0 } },
          { yage::core::key::NUM_LOCK, { "NUM_LOCK", 0 } },

          /* Keypad functions */
          { yage::core::key::KP_SPACE, { "KP_SPACE", ' ' } },
          { yage::core::key::KP_TAB, { "KP_TAB", '\t' } },
          { yage::core::key::KP_ENTER, { "KP_ENTER", 0 } },
          { yage::core::key::KP_HOME, { "KP_HOME", 0 } },
          { yage::core::key::KP_LEFT, { "KP_LEFT", 0 } },
          { yage::core::key::KP_UP, { "KP_UP", 0 } },
          { yage::core::key::KP_RIGHT, { "KP_RIGHT", 0 } },
          { yage::core::key::KP_DOWN, { "KP_DOWN", 0 } },
          { yage::core::key::KP_PRIOR, { "KP_PRIOR", 0 } },
          { yage::core::key::KP_PAGE_UP, { "KP_PAGE_UP", 0 } },
          { yage::core::key::KP_NEXT, { "KP_NEXT", 0 } },
          { yage::core::key::KP_PAGE_DOWN, { "KP_PAGE_DOWN", 0 } },
          { yage::core::key::KP_END, { "KP_END", 0 } },
          { yage::core::key::KP_BEGIN, { "KP_BEGIN", 0 } },
          { yage::core::key::KP_INSERT, { "KP_INSERT", 0 } },
          { yage::core::key::KP_DELETE, { "KP_DELETE", 0 } },
          { yage::core::key::KP_EQUAL, { "KP_EQUAL", '=' } },
          { yage::core::key::KP_MULTIPLY, { "KP_MULTIPLY", '*' } },
          { yage::core::key::KP_ADD, { "KP_ADD", '+' } },
          { yage::core::key::KP_SEPARATOR, { "KP_SEPARATOR", 0 } },
          { yage::core::key::KP_SUBTRACT, { "KP_SUBTRACT", '-' } },
          { yage::core::key::KP_DECIMAL, { "KP_DECIMAL", '.' } },
          { yage::core::key::KP_DIVIDE, { "KP_DIVIDE", '/' } },
          { yage::core::key::KP_0, { "KP_0", '0' } },
          { yage::core::key::KP_1, { "KP_1", '1' } },
          { yage::core::key::KP_2, { "KP_2", '2' } },
          { yage::core::key::KP_3, { "KP_3", '3' } },
          { yage::core::key::KP_4, { "KP_4", '4' } },
          { yage::core::key::KP_5, { "KP_5", '5' } },
          { yage::core::key::KP_6, { "KP_6", '6' } },
          { yage::core::key::KP_7, { "KP_7", '7' } },
          { yage::core::key::KP_8, { "KP_8", '8' } },
          { yage::core::key::KP_9, { "KP_9", '9' } },

          /* Function keys */
          { yage::core::key::F1, { "F1", 0 } },
          { yage::core::key::F2, { "F2", 0 } },
          { yage::core::key::F3, { "F3", 0 } },
          { yage::core::key::F4, { "F4", 0 } },
          { yage::core::key::F5, { "F5", 0 } },
          { yage::core::key::F6, { "F6", 0 } },
          { yage::core::key::F7, { "F7", 0 } },
          { yage::core::key::F8, { "F8", 0 } },
          { yage::core::key::F9, { "F9", 0 } },
          { yage::core::key::F10, { "F10", 0 } },
          { yage::core::key::F11, { "F11", 0 } },
          { yage::core::key::F12, { "F12", 0 } },

          /* Key state modifier keys */
          { yage::core::key::SHIFT_L, { "SHIFT_L", 0 } },
          { yage::core::key::SHIFT_R, { "SHIFT_R", 0 } },
          { yage::core::key::CONTROL_L, { "CONTROL_L", 0 } },
          { yage::core::key::CONTROL_R, { "CONTROL_R", 0 } },
          { yage::core::key::CAPS_LOCK, { "CAPS_LOCK", 0 } },
          { yage::core::key::SHIFT_LOCK, { "SHIFT_LOCK", 0 } },
          { yage::core::key::META_L, { "META_L", 0 } },
          { yage::core::key::META_R, { "META_R", 0 } },
          { yage::core::key::ALT_L, { "ALT_L", 0 } },
          { yage::core::key::ALT_R, { "ALT_R", 0 } },
          { yage::core::key::SUPER_L, { "SUPER_L", 0 } },
          { yage::core::key::SUPER_R, { "SUPER_R", 0 } },
          { yage::core::key::HYPER_L, { "HYPER_L", 0 } },
          { yage::core::key::HYPER_R, { "HYPER_R", 0 } },

          /* Reserved */
          { yage::core::key::OEM_X1, { "OEM_X1", 0 } },
          { yage::core::key::OEM_X2, { "OEM_X2", 0 } },
          { yage::core::key::OEM_X3, { "OEM_X3", 0 } },
          { yage::core::key::OEM_X4, { "OEM_X4", 0 } },
          { yage::core::key::OEM_X5, { "OEM_X5", 0 } },
          { yage::core::key::OEM_X6, { "OEM_X6", 0 } },
          { yage::core::key::OEM_X7, { "OEM_X7", 0 } },
          { yage::core::key::OEM_X8, { "OEM_X8", 0 } },
          { yage::core::key::OEM_X9, { "OEM_X9", 0 } },
          { yage::core::key::OEM_X11, { "OEM_X11", 0 } },
          { yage::core::key::OEM_X12, { "OEM_X12", 0 } },
          { yage::core::key::OEM_X13, { "OEM_X13", 0 } },
          { yage::core::key::OEM_X14, { "OEM_X14", 0 } },
          { yage::core::key::OEM_X15, { "OEM_X15", 0 } },
          { yage::core::key::OEM_X16, { "OEM_X16", 0 } },
          { yage::core::key::OEM_X17, { "OEM_X17", 0 } },
          { yage::core::key::OEM_X18, { "OEM_X18", 0 } },
          { yage::core::key::OEM_X19, { "OEM_X19", 0 } },
        };

/* Shared functions ========================================================= */
std::string yage::core::key_to_string(yage::core::key k)
{
  auto it = key_descriptors.find(k);
  if (it == key_descriptors.end())
  {
    return "UNKNOWN";
  }

  return it->second.name;
}

char yage::core::key_to_char(yage::core::key k)
{
  auto it = key_descriptors.find(k);
  if (it == key_descriptors.end())
  {
    return 0;
  }

  return it->second.code;
}

yage::core::key yage::core::key_from_string(const std::string & str)
{
  for (const auto & [key, descriptor] : key_descriptors)
  {
    if (descriptor.name == str)
    {
      return key;
    }
  }

  return yage::core::key::UNKNOWN;
}
