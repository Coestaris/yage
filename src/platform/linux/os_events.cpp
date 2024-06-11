/** ========================================================================
 *
 * @file os_events.cpp
 * @date 10-06-2024
 * @author Maxim Kurylko <vk_vm@ukr.net>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <csignal>
#include <execinfo.h>
#include "core/os_events.hpp"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
yage::core::os_events * instance = nullptr;

/* Static functions ========================================================= */
static void stop_handler(int signal)
{
  log(LOG_WARN, "Received stop signal: %d", signal);
  if (instance)
  {
    yage::core::event<int> event(signal);
    instance->on_stop_signal.publish(event);
  }
}

void handler(int signal, siginfo_t * info, void * context)
{
  log(LOG_FATAL, "Received illness signal: %d", signal);

  if (instance)
  {
    yage::core::event<int> event(signal);
    instance->on_ill_signal.publish(event);
  }

  struct sigaction action;
  action.sa_handler   = SIG_DFL;
  action.sa_sigaction = NULL;
  action.sa_flags     = 0;
  action.sa_restorer  = NULL;

  fprintf(stderr, "Fault address: %p\n", info->si_addr);
  switch (info->si_code)
  {
    case SEGV_MAPERR:
      fprintf(stderr, "Address not mapped.\n");
      break;

    case SEGV_ACCERR:
      fprintf(stderr, "Access to this address is not allowed.\n");
      break;

    default:
      fprintf(stderr, "Unknown reason.\n");
      break;
  }

  void * array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  /* Dump registers */
  ucontext_t * uc = (ucontext_t *)context;
  fprintf(stderr, "R8:  %016llx\n", uc->uc_mcontext.gregs[REG_R8]);
  fprintf(stderr, "R9:  %016llx\n", uc->uc_mcontext.gregs[REG_R9]);
  fprintf(stderr, "R10: %016llx\n", uc->uc_mcontext.gregs[REG_R10]);
  fprintf(stderr, "R11: %016llx\n", uc->uc_mcontext.gregs[REG_R11]);
  fprintf(stderr, "R12: %016llx\n", uc->uc_mcontext.gregs[REG_R12]);
  fprintf(stderr, "R13: %016llx\n", uc->uc_mcontext.gregs[REG_R13]);
  fprintf(stderr, "R14: %016llx\n", uc->uc_mcontext.gregs[REG_R14]);
  fprintf(stderr, "R15: %016llx\n", uc->uc_mcontext.gregs[REG_R15]);
  fprintf(stderr, "RDI: %016llx\n", uc->uc_mcontext.gregs[REG_RDI]);
  fprintf(stderr, "RSI: %016llx\n", uc->uc_mcontext.gregs[REG_RSI]);
  fprintf(stderr, "RBP: %016llx\n", uc->uc_mcontext.gregs[REG_RBP]);
  fprintf(stderr, "RBX: %016llx\n", uc->uc_mcontext.gregs[REG_RBX]);
  fprintf(stderr, "RDX: %016llx\n", uc->uc_mcontext.gregs[REG_RDX]);
  fprintf(stderr, "RAX: %016llx\n", uc->uc_mcontext.gregs[REG_RAX]);
  fprintf(stderr, "RCX: %016llx\n", uc->uc_mcontext.gregs[REG_RCX]);
  fprintf(stderr, "RSP: %016llx\n", uc->uc_mcontext.gregs[REG_RSP]);
  fprintf(stderr, "RIP: %016llx\n", uc->uc_mcontext.gregs[REG_RIP]);

  /* print out all the frames to stderr */
  printf("Obtained %zd stack frames.\n", size);
  backtrace_symbols_fd(array, size, 2);

  /* unregister and let the default action occur */
  sigaction(SIGSEGV, &action, nullptr);

  struct sigaction sa = {};
  sigaction(SIGABRT, nullptr, &sa);
  sa.sa_handler = SIG_DFL;
  sigaction(SIGABRT, &sa, nullptr);
  abort();
}

/* Shared functions ========================================================= */
yage::core::os_events::os_events()
{
  if (instance != nullptr)
  {
    throw std::runtime_error("Linux implementation of os_events is singleton");
  }

  instance = this;
  std::signal(SIGINT, stop_handler);
  std::signal(SIGSTOP, stop_handler);
  std::signal(SIGTERM, stop_handler);

  struct sigaction action;
  action.sa_sigaction = handler;
  action.sa_flags     = SA_SIGINFO;
  sigemptyset(&action.sa_mask);

  sigaction(SIGILL, &action, nullptr);
  sigaction(SIGABRT, &action, nullptr);
  sigaction(SIGBUS, &action, nullptr);
  sigaction(SIGFPE, &action, nullptr);
  sigaction(SIGSEGV, &action, nullptr);
}

void yage::core::os_events::update()
{
}
