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

#include "core/io/keyboard.hpp"

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
static const std::unordered_map<yage::core::io::key, key_descriptor>
      key_descriptors = {
        { yage::core::io::key::UNKNOWN, { "UNKNOWN", 0 } },

        /* Control keys */
        { yage::core::io::key::ENTER, { "ENTER", 0 } },
        { yage::core::io::key::DEL, { "DEL", 0 } },
        { yage::core::io::key::BACKSPACE, { "BACKSPACE", 0 } },
        { yage::core::io::key::ESCAPE, { "ESCAPE", 0 } },
        { yage::core::io::key::TAB, { "TAB", 0 } },
        { yage::core::io::key::SPACE, { "SPACE", 0 } },
        { yage::core::io::key::SCROLL_LOCK, { "SCROLL_LOCK", 0 } },
        { yage::core::io::key::PAUSE, { "PAUSE", 0 } },
        { yage::core::io::key::SYS_REQ, { "SYS_REQ", 0 } },
        { yage::core::io::key::HOME, { "HOME", 0 } },
        { yage::core::io::key::LEFT, { "LEFT", 0 } },
        { yage::core::io::key::UP, { "UP", 0 } },
        { yage::core::io::key::RIGHT, { "RIGHT", 0 } },
        { yage::core::io::key::DOWN, { "DOWN", 0 } },
        { yage::core::io::key::PRIOR, { "PRIOR", 0 } },
        { yage::core::io::key::PAGE_UP, { "PAGE_UP", 0 } },
        { yage::core::io::key::NEXT, { "NEXT", 0 } },
        { yage::core::io::key::PAGE_DOWN, { "PAGE_DOWN", 0 } },
        { yage::core::io::key::END, { "END", 0 } },
        { yage::core::io::key::BEGIN, { "BEGIN", 0 } },

        /* Special keys */
        { yage::core::io::key::SK1, { "SK1", 0 } },
        { yage::core::io::key::SK2, { "SK2", 0 } },
        { yage::core::io::key::SK3, { "SK3", 0 } },
        { yage::core::io::key::SK4, { "SK4", 0 } },
        { yage::core::io::key::SK5, { "SK5", 0 } },
        { yage::core::io::key::SK6, { "SK6", 0 } },
        { yage::core::io::key::SK7, { "SK7", 0 } },
        { yage::core::io::key::SK8, { "SK8", 0 } },
        { yage::core::io::key::SK9, { "SK9", 0 } },
        { yage::core::io::key::SK10, { "SK10", 0 } },
        { yage::core::io::key::SK11, { "SK11", 0 } },

        /* Letters */
        { yage::core::io::key::A, { "A", 'A' } },
        { yage::core::io::key::B, { "B", 'B' } },
        { yage::core::io::key::C, { "C", 'C' } },
        { yage::core::io::key::D, { "D", 'D' } },
        { yage::core::io::key::E, { "E", 'E' } },
        { yage::core::io::key::F, { "F", 'F' } },
        { yage::core::io::key::G, { "G", 'G' } },
        { yage::core::io::key::H, { "H", 'H' } },
        { yage::core::io::key::I, { "I", 'I' } },
        { yage::core::io::key::J, { "J", 'J' } },
        { yage::core::io::key::K, { "K", 'K' } },
        { yage::core::io::key::L, { "L", 'L' } },
        { yage::core::io::key::M, { "M", 'M' } },
        { yage::core::io::key::N, { "N", 'N' } },
        { yage::core::io::key::O, { "O", 'O' } },
        { yage::core::io::key::P, { "P", 'P' } },
        { yage::core::io::key::Q, { "Q", 'Q' } },
        { yage::core::io::key::R, { "R", 'R' } },
        { yage::core::io::key::S, { "S", 'S' } },
        { yage::core::io::key::T, { "T", 'T' } },
        { yage::core::io::key::U, { "U", 'U' } },
        { yage::core::io::key::V, { "V", 'V' } },
        { yage::core::io::key::W, { "W", 'W' } },
        { yage::core::io::key::X, { "X", 'X' } },
        { yage::core::io::key::Y, { "Y", 'Y' } },
        { yage::core::io::key::Z, { "Z", 'Z' } },

        { yage::core::io::key::K0, { "0", '0' } },
        { yage::core::io::key::K1, { "1", '1' } },
        { yage::core::io::key::K2, { "2", '2' } },
        { yage::core::io::key::K3, { "3", '3' } },
        { yage::core::io::key::K4, { "4", '4' } },
        { yage::core::io::key::K5, { "5", '5' } },
        { yage::core::io::key::K6, { "6", '6' } },
        { yage::core::io::key::K7, { "7", '7' } },
        { yage::core::io::key::K8, { "8", '8' } },
        { yage::core::io::key::K9, { "9", '9' } },

        /* Misc functions */
        { yage::core::io::key::SELECT, { "SELECT", 0 } },
        { yage::core::io::key::PRINT, { "PRINT", 0 } },
        { yage::core::io::key::EXECUTE, { "EXECUTE", 0 } },
        { yage::core::io::key::INSERT, { "INSERT", 0 } },
        { yage::core::io::key::UNDO, { "UNDO", 0 } },
        { yage::core::io::key::REDO, { "REDO", 0 } },
        { yage::core::io::key::MENU, { "MENU", 0 } },
        { yage::core::io::key::FIND, { "FIND", 0 } },
        { yage::core::io::key::CANCEL, { "CANCEL", 0 } },
        { yage::core::io::key::HELP, { "HELP", 0 } },
        { yage::core::io::key::BREAK, { "BREAK", 0 } },
        { yage::core::io::key::CLEAR, { "CLEAR", 0 } },
        { yage::core::io::key::NUM_LOCK, { "NUM_LOCK", 0 } },

        /* Keypad functions */
        { yage::core::io::key::KP_SPACE, { "KP_SPACE", ' ' } },
        { yage::core::io::key::KP_TAB, { "KP_TAB", '\t' } },
        { yage::core::io::key::KP_ENTER, { "KP_ENTER", 0 } },
        { yage::core::io::key::KP_HOME, { "KP_HOME", 0 } },
        { yage::core::io::key::KP_LEFT, { "KP_LEFT", 0 } },
        { yage::core::io::key::KP_UP, { "KP_UP", 0 } },
        { yage::core::io::key::KP_RIGHT, { "KP_RIGHT", 0 } },
        { yage::core::io::key::KP_DOWN, { "KP_DOWN", 0 } },
        { yage::core::io::key::KP_PRIOR, { "KP_PRIOR", 0 } },
        { yage::core::io::key::KP_PAGE_UP, { "KP_PAGE_UP", 0 } },
        { yage::core::io::key::KP_NEXT, { "KP_NEXT", 0 } },
        { yage::core::io::key::KP_PAGE_DOWN, { "KP_PAGE_DOWN", 0 } },
        { yage::core::io::key::KP_END, { "KP_END", 0 } },
        { yage::core::io::key::KP_BEGIN, { "KP_BEGIN", 0 } },
        { yage::core::io::key::KP_INSERT, { "KP_INSERT", 0 } },
        { yage::core::io::key::KP_DELETE, { "KP_DELETE", 0 } },
        { yage::core::io::key::KP_EQUAL, { "KP_EQUAL", '=' } },
        { yage::core::io::key::KP_MULTIPLY, { "KP_MULTIPLY", '*' } },
        { yage::core::io::key::KP_ADD, { "KP_ADD", '+' } },
        { yage::core::io::key::KP_SEPARATOR, { "KP_SEPARATOR", 0 } },
        { yage::core::io::key::KP_SUBTRACT, { "KP_SUBTRACT", '-' } },
        { yage::core::io::key::KP_DECIMAL, { "KP_DECIMAL", '.' } },
        { yage::core::io::key::KP_DIVIDE, { "KP_DIVIDE", '/' } },
        { yage::core::io::key::KP_0, { "KP_0", '0' } },
        { yage::core::io::key::KP_1, { "KP_1", '1' } },
        { yage::core::io::key::KP_2, { "KP_2", '2' } },
        { yage::core::io::key::KP_3, { "KP_3", '3' } },
        { yage::core::io::key::KP_4, { "KP_4", '4' } },
        { yage::core::io::key::KP_5, { "KP_5", '5' } },
        { yage::core::io::key::KP_6, { "KP_6", '6' } },
        { yage::core::io::key::KP_7, { "KP_7", '7' } },
        { yage::core::io::key::KP_8, { "KP_8", '8' } },
        { yage::core::io::key::KP_9, { "KP_9", '9' } },

        /* Function keys */
        { yage::core::io::key::F1, { "F1", 0 } },
        { yage::core::io::key::F2, { "F2", 0 } },
        { yage::core::io::key::F3, { "F3", 0 } },
        { yage::core::io::key::F4, { "F4", 0 } },
        { yage::core::io::key::F5, { "F5", 0 } },
        { yage::core::io::key::F6, { "F6", 0 } },
        { yage::core::io::key::F7, { "F7", 0 } },
        { yage::core::io::key::F8, { "F8", 0 } },
        { yage::core::io::key::F9, { "F9", 0 } },
        { yage::core::io::key::F10, { "F10", 0 } },
        { yage::core::io::key::F11, { "F11", 0 } },
        { yage::core::io::key::F12, { "F12", 0 } },

        /* Key state modifier keys */
        { yage::core::io::key::SHIFT_L, { "SHIFT_L", 0 } },
        { yage::core::io::key::SHIFT_R, { "SHIFT_R", 0 } },
        { yage::core::io::key::CONTROL_L, { "CONTROL_L", 0 } },
        { yage::core::io::key::CONTROL_R, { "CONTROL_R", 0 } },
        { yage::core::io::key::CAPS_LOCK, { "CAPS_LOCK", 0 } },
        { yage::core::io::key::SHIFT_LOCK, { "SHIFT_LOCK", 0 } },
        { yage::core::io::key::META_L, { "META_L", 0 } },
        { yage::core::io::key::META_R, { "META_R", 0 } },
        { yage::core::io::key::ALT_L, { "ALT_L", 0 } },
        { yage::core::io::key::ALT_R, { "ALT_R", 0 } },
        { yage::core::io::key::SUPER_L, { "SUPER_L", 0 } },
        { yage::core::io::key::SUPER_R, { "SUPER_R", 0 } },
        { yage::core::io::key::HYPER_L, { "HYPER_L", 0 } },
        { yage::core::io::key::HYPER_R, { "HYPER_R", 0 } },

        /* Reserved */
        { yage::core::io::key::OEM_X1, { "OEM_X1", 0 } },
        { yage::core::io::key::OEM_X2, { "OEM_X2", 0 } },
        { yage::core::io::key::OEM_X3, { "OEM_X3", 0 } },
        { yage::core::io::key::OEM_X4, { "OEM_X4", 0 } },
        { yage::core::io::key::OEM_X5, { "OEM_X5", 0 } },
        { yage::core::io::key::OEM_X6, { "OEM_X6", 0 } },
        { yage::core::io::key::OEM_X7, { "OEM_X7", 0 } },
        { yage::core::io::key::OEM_X8, { "OEM_X8", 0 } },
        { yage::core::io::key::OEM_X9, { "OEM_X9", 0 } },
        { yage::core::io::key::OEM_X11, { "OEM_X11", 0 } },
        { yage::core::io::key::OEM_X12, { "OEM_X12", 0 } },
        { yage::core::io::key::OEM_X13, { "OEM_X13", 0 } },
        { yage::core::io::key::OEM_X14, { "OEM_X14", 0 } },
        { yage::core::io::key::OEM_X15, { "OEM_X15", 0 } },
        { yage::core::io::key::OEM_X16, { "OEM_X16", 0 } },
        { yage::core::io::key::OEM_X17, { "OEM_X17", 0 } },
        { yage::core::io::key::OEM_X18, { "OEM_X18", 0 } },
        { yage::core::io::key::OEM_X19, { "OEM_X19", 0 } },
      };

/* Shared functions ========================================================= */
std::string yage::core::io::key_to_string(yage::core::io::key k)
{
  auto it = key_descriptors.find(k);
  if (it == key_descriptors.end())
  {
    return "UNKNOWN";
  }

  return it->second.name;
}

char yage::core::io::key_to_char(yage::core::io::key k)
{
  auto it = key_descriptors.find(k);
  if (it == key_descriptors.end())
  {
    return 0;
  }

  return it->second.code;
}

yage::core::io::key yage::core::io::key_from_string(const std::string & str)
{
  for (const auto & [key, descriptor] : key_descriptors)
  {
    if (descriptor.name == str)
    {
      return key;
    }
  }

  return yage::core::io::key::UNKNOWN;
}
