/** ========================================================================
 *
 * @file named_thread.cpp
 * @date 01-07-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <windows.h>
#include "yagecore/named_thread.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO
{
  DWORD  dwType;     // Must be 0x1000.
  LPCSTR szName;     // Pointer to name (in user addr space).
  DWORD  dwThreadID; // Thread ID (-1=caller thread).
  DWORD  dwFlags;    // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

/* Variables ================================================================ */
const DWORD MS_VC_EXCEPTION = 0x406D1388;

/* Static functions ========================================================= */
/* Shared functions ========================================================= */
void yc::named_thread::set_port_name()
{
  /* Note that this function is not portable, and
   * should be implemented for each platform separately. */
  THREADNAME_INFO info;
  info.dwType     = 0x1000;
  info.szName     = name.c_str();
  info.dwThreadID = thread->native_handle();
  info.dwFlags    = 0;

  RaiseException(
        MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR),
        reinterpret_cast<ULONG_PTR *>(&info));
}
