/* Copyright 2019 Nikoli Dryden
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define _XOPEN_SOURCE 500  // For additional signal information.
#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "shbt/shbt.h"
#include "shbt/shbt_internal.h"

#ifdef SHBT_HAVE_MPI
#include <mpi.h>
#endif

// Signal information and bookkeeping.
static struct shbt_signal_info sig_info[] = {
#ifdef SIGABRT
  {SIGABRT, "ABRT", "Abort signal", 0, NULL},
#endif
#ifdef SIGALRM
  {SIGALRM, "ALRM", "Timer signal", 0, NULL},
#endif
#ifdef SIGBUS
  {SIGBUS, "BUS", "Bus error", 0, NULL},
#endif
#ifdef SIGCHLD
  {SIGCHLD, "CHLD", "Child stopped or terminated", 0, NULL},
#endif
#ifdef SIGCLD
  {SIGCLD, "CLD", "Child stopped or terminated", 0, NULL},
#endif
#ifdef SIGCONT
  {SIGCONT, "CONT", "Continue if stopped", 0, NULL},
#endif
#ifdef SIGEMT
  {SIGEMT, "EMT", "Emulator trap", 0, NULL},
#endif
#ifdef SIGFPE
  {SIGFPE, "FPE", "Floating-point exception", 0, NULL},
#endif
#ifdef SIGHUP
  {SIGHUP, "HUP", "Hangup detected", 0, NULL},
#endif
#ifdef SIGILL
  {SIGILL, "ILL", "Illegal instruction", 0, NULL},
#endif
#ifdef SIGINFO
  {SIGINFO, "INFO", "Power failure", 0, NULL},
#endif
#ifdef SIGINT
  {SIGINT, "INT", "Interrupt", 0, NULL},
#endif
#ifdef SIGIO
  {SIGIO, "IO", "I/O now possible", 0, NULL},
#endif
#ifdef SIGIOT
  {SIGIOT, "IOT", "IOT trap (abort)", 0, NULL},
#endif
#ifdef SIGKILL
  {SIGKILL, "KILL", "Kill", 0, NULL},
#endif
#ifdef SIGLOST
  {SIGLOST, "LOST", "File lock lost", 0, NULL},
#endif
#ifdef SIGPIPE
  {SIGPIPE, "PIPE", "Broken pipe", 0, NULL},
#endif
#ifdef SIGPOLL
  {SIGPOLL, "POLL", "Pollable event", 0, NULL},
#endif
#ifdef SIGPROF
  {SIGPROF, "PROF", "Profiling timer expired", 0, NULL},
#endif
#ifdef SIGPWR
  {SIGPWR, "PWR", "Power failure", 0, NULL},
#endif
#ifdef SIGQUIT
  {SIGQUIT, "QUIT", "Quit", 0, NULL},
#endif
#ifdef SIGSEGV
  {SIGSEGV, "SEGV", "Invalid memory reference", 0, NULL},
#endif
#ifdef SIGSTKFLT
  {SIGSTKFLT, "STKFLT", "Stack fault on coprocessor", 0, NULL},
#endif
#ifdef SIGSTOP
  {SIGSTOP, "STOP", "Stop process", 0, NULL},
#endif
#ifdef SIGTSTP
  {SIGTSTP, "TSTP", "Stop typed at terminal", 0, NULL},
#endif
#ifdef SIGSYS
  {SIGSYS, "SYS", "Bad system call", 0, NULL},
#endif
#ifdef SIGTERM
  {SIGTERM, "TERM", "Terminate", 0, NULL},
#endif
#ifdef SIGTRAP
  {SIGTRAP, "TRAP", "Trace/breakpoint trap", 0, NULL},
#endif
#ifdef SIGTTIN
  {SIGTTIN, "TTIN", "Terminal input for background process", 0, NULL},
#endif
#ifdef SIGTTOU
  {SIGTTOU, "TTOU", "Terminal output for background process", 0, NULL},
#endif
#ifdef SIGURG
  {SIGURG, "URG", "Urgent condition on socket", 0, NULL},
#endif
#ifdef SIGUSR1
  {SIGUSR1, "USR1", "User-defined signal 1", 0, NULL},
#endif
#ifdef SIGUSR2
  {SIGUSR2, "USR2", "User-defined signal 2", 0, NULL},
#endif
#ifdef SIGVTALRM
  {SIGVTALRM, "VTALRM", "Virtual alarm clock", 0, NULL},
#endif
#ifdef SIGXCPU
  {SIGXCPU, "XCPU", "CPU time limit exceeded", 0, NULL},
#endif
#ifdef SIGXFSZ
  {SIGXFSZ, "XFSZ", "File size limit exceeded", 0, NULL},
#endif
#ifdef SIGWINCH
  {SIGWINCH, "WINCH", "Window resize", 0, NULL},
#endif
  {0, NULL, NULL, 0, NULL}};

// Generic signal codes.
static const struct shbt_signal_code_info generic_codes[] = {
#ifdef SI_USER
  {SI_USER, "USER", "Signal sent via kill"},
#endif
#ifdef SI_KERNEL
  {SI_KERNEL, "KERNEL", "Signal sent by the kernel"},
#endif
#ifdef SI_QUEUE
  {SI_QUEUE, "QUEUE", "Signal sent via sigqueue"},
#endif
#ifdef SI_TIMER
  {SI_TIMER, "TIMER", "POSIX timer expired"},
#endif
#ifdef SI_MESGQ
  {SI_MESGQ, "MESGQ", "POSIX message queue state changed"},
#endif
#ifdef SI_ASYNCIO
  {SI_ASYNCIO, "ASYNCIO", "AIO completed"},
#endif
#ifdef SI_SIGIO
  {SI_SIGIO, "SIGIO", "Queued SIGIO"},
#endif
#ifdef SI_TKILL
  {SI_TKILL, "TKILL", "Signal sent via tkill/tgkill"},
#endif
  {0, NULL, NULL}};

// Information for signal codes for particular signals.
#ifdef SIGILL
static const struct shbt_signal_code_info sigill_codes[] = {
#ifdef ILL_ILLOPC
  {ILL_ILLOPC, "ILLOPC", "Illegal opcode"},
#endif
#ifdef ILL_ILLOPN
  {ILL_ILLOPN, "ILLOPN", "Illegal operand"},
#endif
#ifdef ILL_ILLADR
  {ILL_ILLADR, "ILLADR", "Illegal addressing mode"},
#endif
#ifdef ILL_ILLTRP
  {ILL_ILLTRP, "ILLTRP", "Illegal trap"},
#endif
#ifdef ILL_PRVOPC
  {ILL_PRVOPC, "PRVOPC", "Privileged opcode"},
#endif
#ifdef ILL_PRVREG
  {ILL_PRVREG, "PRVREG", "Privileged register"},
#endif
#ifdef ILL_COPROC
  {ILL_COPROC, "COPROC", "Coprocessor error"},
#endif
#ifdef ILL_BADSTK
  {ILL_BADSTK, "BADSTK", "Internal stack error"},
#endif
  {0, NULL, NULL}};
#endif  // SIGILL
#ifdef SIGFPE
static const struct shbt_signal_code_info sigfpe_codes[] = {
#ifdef FPE_INTDIV
  {FPE_INTDIV, "INTDIV", "Integer divide by zero"},
#endif
#ifdef FPE_INTOVF
  {FPE_INTOVF, "INTOVF", "Integer overflow"},
#endif
#ifdef FPE_FLTDIV
  {FPE_FLTDIV, "FLTDIV", "Floating-point divide by zero"},
#endif
#ifdef FPE_FLTOVF
  {FPE_FLTOVF, "FLTOVF", "Floating-point overflow"},
#endif
#ifdef FPE_FLTUND
  {FPE_FLTUND, "FLTUND", "Floating-point underflow"},
#endif
#ifdef FPE_FLTRES
  {FPE_FLTRES, "FLTRES", "Floating-point inexact result"},
#endif
#ifdef FPE_FLTINV
  {FPE_FLTINV, "FLTINV", "Floating-point invalid operation"},
#endif
#ifdef FPE_FLTSUB
  {FPE_FLTSUB, "FLTSUB", "Subscript out of range"},
#endif
  {0, NULL, NULL}};
#endif  // SIGFPE
#ifdef SIGSEGV
static const struct shbt_signal_code_info sigsegv_codes[] = {
#ifdef SEGV_MAPERR
  {SEGV_MAPERR, "MAPERR", "Address not mapped to object"},
#endif
#ifdef SEGV_ACCERR
  {SEGV_ACCERR, "ACCERR", "Invalid permissions for mapped object"},
#endif
#ifdef SEGV_BNDERR
  {SEGV_BNDERR, "BNDERR", "Failed address bound checks"},
#endif
#ifdef SEGV_PKUERR
  {SEGV_PKUERR, "PKUERR", "Access denied by memory protection keys"},
#endif
  {0, NULL, NULL}};
#endif  // SIGSEGV
#ifdef SIGBUS
static const struct shbt_signal_code_info sigbus_codes[] = {
#ifdef BUS_ADRALN
  {BUS_ADRALN, "ADRALN", "Invalid address alignment"},
#endif
#ifdef BUS_ADRERR
  {BUS_ADRERR, "ADRERR", "Nonexistent physical address"},
#endif
#ifdef BUS_OBJERR
  {BUS_OBJERR, "OBJERR", "Object-specific hardware error"},
#endif
#ifdef BUS_MCEERR_AR
  {BUS_MCEERR_AR, "MCEERR_AR",
   "Hardware memory error consumed on a machine check"},
#endif
#ifdef BUS_MCEERR_AO
  {BUS_MCEERR_AO, "MCEERR_AO",
   "Hardware memory error detected in process but not consumed"},
#endif
  {0, NULL, NULL}};
#endif  // SIGBUS
#ifdef SIGTRAP
static const struct shbt_signal_code_info sigtrap_codes[] = {
#ifdef TRAP_BRKPT
  {TRAP_BRKPT, "BRKPT", "Process breakpoint"},
#endif
#ifdef TRAP_TRACE
  {TRAP_TRACE, "TRACE", "Process trace trap"},
#endif
#ifdef TRAP_BRANCH
  {TRAP_BRANCH, "BRANCH", "Process taken branch trap"},
#endif
#ifdef TRAP_HWBKPT
  {TRAP_HWBKPT, "HWBKPT", "Hardware breakpoint/watchpoint"},
#endif
  {0, NULL, NULL}};
#endif  // SIGTRAP
#if defined(SIGIO) || defined(SIGPOLL)
static const struct shbt_signal_code_info sigpoll_codes[] = {
#ifdef POLL_IN
  {POLL_IN, "IN", "Data input available"},
#endif
#ifdef POLL_OUT
  {POLL_OUT, "OUT", "Output buffers available"},
#endif
#ifdef POLL_MSG
  {POLL_MSG, "MSG", "Input message available"},
#endif
#ifdef POLL_ERR
  {POLL_ERR, "ERR", "I/O error"},
#endif
#ifdef POLL_PRI
  {POLL_PRI, "PRI", "High priority input available"},
#endif
#ifdef POLL_HUP
  {POLL_HUP, "HUP", "Device disconnected"},
#endif
  {0, NULL, NULL}};
#endif  // defined(SIGIO) || defined(SIGPOLL)
#ifdef SIGSYS
static const struct shbt_signal_code_info sigsys_codes[] = {
#ifdef SYS_SECCOMP
  {SYS_SECCOMP, "SECCOMP", "Triggered by seccomp filter rule"},
#endif
  {0, NULL, NULL}};
#endif

#ifdef SHBT_HAVE_MPI
static int mpi_rank = -1;
#endif

static void* signal_handler_stack = NULL;

struct shbt_signal_info* shbt_get_signal_info(int sig_num) {
  for (size_t i = 0; sig_info[i].sig_name != NULL; ++i) {
    if (sig_info[i].sig_num == sig_num) {
      return &sig_info[i];
    }
  }
  return NULL;
}

const struct shbt_signal_code_info* shbt_get_signal_code_info(
  const struct shbt_signal_code_info info_list[], int code_num) {
  for (size_t i = 0; info_list[i].code_name != NULL; ++i) {
    if (info_list[i].code_num == code_num) {
      return &info_list[i];
    }
  }
  return NULL;
}

void shbt_print_signal(int sig_num, siginfo_t* info) {
  char str_buf[128] = {0};  // Should be large enough for everything.
  shbt_itoa(sig_num, str_buf, sizeof(str_buf), 10, 0);
  struct shbt_signal_info* shbt_info = shbt_get_signal_info(sig_num);
  if (shbt_info == NULL) {
    // No info on what this signal is, so just do our best.
    shbt_print_to_stderr("Received unknown signal ");
    shbt_print_to_stderr(str_buf);
#ifdef SHBT_HAVE_MPI
    if (mpi_rank >= 0) {
      shbt_print_to_stderr(" on rank ");
      shbt_itoa(mpi_rank, str_buf, sizeof(str_buf), 10, 0);
      shbt_print_to_stderr(str_buf);
    }
#endif
    shbt_print_to_stderr("\n");
    return;
  }
  shbt_print_to_stderr("Received signal ");
  shbt_print_to_stderr(str_buf);
  shbt_print_to_stderr(" ");
  shbt_print_to_stderr(shbt_info->sig_name);
  shbt_print_to_stderr(" - ");
  shbt_print_to_stderr(shbt_info->sig_desc);
#ifdef SHBT_HAVE_MPI
  if (mpi_rank >= 0) {
    shbt_print_to_stderr(" on rank ");
    shbt_itoa(mpi_rank, str_buf, sizeof(str_buf), 10, 0);
    shbt_print_to_stderr(str_buf);
  }
#endif
  // Attempt to provide additional information when available.
  // Note: While SIGCHLD does provide additional info, it doesn't make much
  // sense to attempt to interpret it here, since the default action is to
  // ignore it.
  if (info != NULL) {
    // Attempt to gather generic information.
    bool was_code_generic = false;
    {
      const struct shbt_signal_code_info* code_info =
        shbt_get_signal_code_info(generic_codes, info->si_code);
      if (code_info != NULL) {
        was_code_generic = true;
        shbt_print_to_stderr("\n  ");
        shbt_print_to_stderr(code_info->code_name);
        shbt_print_to_stderr(" - ");
        shbt_print_to_stderr(code_info->code_desc);
      } else {
        // Only print a newline if we don't have a generic code here.
        shbt_print_to_stderr("\n");
      }
      // Print PID/UID info for kill/sigqueue.
      // TODO: It would make sense for tgkill to also fill this in, but there
      // is no documentation about that.
      if (
#ifdef SI_USER
        info->si_code == SI_USER ||
#endif
#ifdef SI_QUEUE
        info->si_code == SI_QUEUE
#else
        0
#endif
      ) {
        shbt_print_to_stderr(" - Source PID: ");
        shbt_itoa(info->si_pid, str_buf, sizeof(str_buf), 10, 0);
        shbt_print_to_stderr(str_buf);
        shbt_print_to_stderr(" - UID: ");
        shbt_itoa(info->si_uid, str_buf, sizeof(str_buf), 10, 0);
        shbt_print_to_stderr(str_buf);
      }
    }
#ifdef SIGILL
    if (sig_num == SIGILL) {
      shbt_print_to_stderr("  ");
      const struct shbt_signal_code_info* code_info =
        shbt_get_signal_code_info(sigill_codes, info->si_code);
      if (code_info != NULL) {
        shbt_print_to_stderr(code_info->code_name);
        shbt_print_to_stderr(" - ");
        shbt_print_to_stderr(code_info->code_desc);
      } else if (!was_code_generic) {
        shbt_print_to_stderr("Unknown signal code ");
        shbt_itoa(info->si_code, str_buf, sizeof(str_buf), 10, 0);
        shbt_print_to_stderr(str_buf);
      }
      shbt_print_to_stderr(" - Fault occurred at address 0x");
      shbt_itoa((intptr_t) info->si_addr, str_buf, sizeof(str_buf), 16, 12);
      shbt_print_to_stderr(str_buf);
      shbt_print_to_stderr("\n");
    } else
#endif  // SIGILL
#ifdef SIGFPE
    if (sig_num == SIGFPE) {
      shbt_print_to_stderr("  ");
      const struct shbt_signal_code_info* code_info =
        shbt_get_signal_code_info(sigfpe_codes, info->si_code);
      if (code_info != NULL) {
        shbt_print_to_stderr(code_info->code_name);
        shbt_print_to_stderr(" - ");
        shbt_print_to_stderr(code_info->code_desc);
      } else if (!was_code_generic) {
        shbt_print_to_stderr("Unknown signal code ");
        shbt_itoa(info->si_code, str_buf, sizeof(str_buf), 10, 0);
        shbt_print_to_stderr(str_buf);
      }
      shbt_print_to_stderr(" - Fault occurred at address 0x");
      shbt_itoa((intptr_t) info->si_addr, str_buf, sizeof(str_buf), 16, 12);
      shbt_print_to_stderr(str_buf);
      shbt_print_to_stderr("\n");
    } else
#endif  // SIGFPE
#ifdef SIGSEGV
    if (sig_num == SIGSEGV) {
      shbt_print_to_stderr("  ");
      const struct shbt_signal_code_info* code_info =
        shbt_get_signal_code_info(sigsegv_codes, info->si_code);
      if (code_info != NULL) {
        shbt_print_to_stderr(code_info->code_name);
        shbt_print_to_stderr(" - ");
        shbt_print_to_stderr(code_info->code_desc);
      } else if (!was_code_generic) {
        shbt_print_to_stderr("Unknown signal code ");
        shbt_itoa(info->si_code, str_buf, sizeof(str_buf), 10, 0);
        shbt_print_to_stderr(str_buf);
      }
      shbt_print_to_stderr(" - Fault occurred at address 0x");
      shbt_itoa((intptr_t) info->si_addr, str_buf, sizeof(str_buf), 16, 12);
      shbt_print_to_stderr(str_buf);
      shbt_print_to_stderr("\n");
    } else
#endif  // SIGSEGV
#ifdef SIGBUS
    if (sig_num == SIGBUS) {
      shbt_print_to_stderr("  ");
      const struct shbt_signal_code_info* code_info =
        shbt_get_signal_code_info(sigbus_codes, info->si_code);
      if (code_info != NULL) {
        shbt_print_to_stderr(code_info->code_name);
        shbt_print_to_stderr(" - ");
        shbt_print_to_stderr(code_info->code_desc);
      } else if (!was_code_generic) {
        shbt_print_to_stderr("Unknown signal code ");
        shbt_itoa(info->si_code, str_buf, sizeof(str_buf), 10, 0);
        shbt_print_to_stderr(str_buf);
      }
      shbt_print_to_stderr(" - Fault occurred at address 0x");
      shbt_itoa((intptr_t) info->si_addr, str_buf, sizeof(str_buf), 16, 12);
      shbt_print_to_stderr(str_buf);
      shbt_print_to_stderr("\n");
    } else
#endif  // SIGBUS
#ifdef SIGTRAP
    if (sig_num == SIGTRAP) {
      shbt_print_to_stderr("  ");
      const struct shbt_signal_code_info* code_info =
        shbt_get_signal_code_info(sigtrap_codes, info->si_code);
      if (code_info != NULL) {
        shbt_print_to_stderr(code_info->code_name);
        shbt_print_to_stderr(" - ");
        shbt_print_to_stderr(code_info->code_desc);
      } else if (!was_code_generic) {
        shbt_print_to_stderr("Unknown signal code ");
        shbt_itoa(info->si_code, str_buf, sizeof(str_buf), 10, 0);
        shbt_print_to_stderr(str_buf);
      }
      shbt_print_to_stderr(" - Fault occurred at address 0x");
      shbt_itoa((intptr_t) info->si_addr, str_buf, sizeof(str_buf), 16, 12);
      shbt_print_to_stderr(str_buf);
      shbt_print_to_stderr("\n");
    } else
#endif  // SIGTRAP
#if defined(SIGIO) || defined(SIGPOLL)
    if (
#ifdef SIGIO
        sig_num == SIGIO ||
#endif
#ifdef SIGPOLL
        sig_num == SIGPOLL
#else
        0
#endif
      ) {
      shbt_print_to_stderr("  ");
      const struct shbt_signal_code_info* code_info =
        shbt_get_signal_code_info(sigpoll_codes, info->si_code);
      if (code_info != NULL) {
        shbt_print_to_stderr(code_info->code_name);
        shbt_print_to_stderr(" - ");
        shbt_print_to_stderr(code_info->code_desc);
      } else if (!was_code_generic) {
        shbt_print_to_stderr("Unknown signal code ");
        shbt_itoa(info->si_code, str_buf, sizeof(str_buf), 10, 0);
        shbt_print_to_stderr(str_buf);
      }
      shbt_print_to_stderr("\n");
    } else
#endif  // defined(SIGIO) || defined(SIGPOLL)
#ifdef SIGSYS
    if (sig_num == SIGSYS) {
      shbt_print_to_stderr("  ");
      const struct shbt_signal_code_info* code_info =
        shbt_get_signal_code_info(sigsys_codes, info->si_code);
      if (code_info != NULL) {
        shbt_print_to_stderr(code_info->code_name);
        shbt_print_to_stderr(" - ");
        shbt_print_to_stderr(code_info->code_desc);
      } else if (!was_code_generic) {
        shbt_print_to_stderr("Unknown signal code ");
        shbt_itoa(info->si_code, str_buf, sizeof(str_buf), 10, 0);
        shbt_print_to_stderr(str_buf);
      }
      shbt_print_to_stderr("\n");
    } else
#endif  // SIGSYS
    {
      // No special info available.
      if (was_code_generic) {
        // Add newline for generic code.
        shbt_print_to_stderr("\n");
      }
    }
  }
}

void shbt_sigaction_handler(int sig_num, siginfo_t* info, void* void_ucontext) {
  (void) void_ucontext;
  struct shbt_signal_info* sig_info = shbt_get_signal_info(sig_num);
  if (sig_info == NULL) {
    // This should never happen, since this signal handler shouldn't be
    // registered if we can't get the SHBT signal info struct.
    shbt_print_to_stderr("SHBT: Could not get signal info in signal handler\n");
    _exit(EXIT_FAILURE);
  }
  shbt_print_signal(sig_num, info);
  shbt_print_to_stderr("Backtrace:\n");
  shbt_print_backtrace_fd(STDERR_FILENO);
  if (sig_info->callback != NULL) {
    sig_info->callback(sig_num);
  }
  if (sig_info->exit_action == SHBT_EXIT_ACTION_EXIT) {
    _exit(EXIT_FAILURE);
  } else if (sig_info->exit_action == SHBT_EXIT_ACTION_RETURN) {
    return;
  } else if (sig_info->exit_action == SHBT_EXIT_ACTION_RERAISE) {
    // Restore the default signal handler.
    struct sigaction sa;
    sa.sa_handler = SIG_DFL;
    sigfillset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(sig_num, &sa, NULL) < 0) {
      shbt_print_to_stderr(
        "SHBT: Error trying to restore default signal handler\n");
      _exit(EXIT_FAILURE);
    }
    raise(sig_num);  // Reraise the signal for the default handler.
  } else {
    shbt_print_to_stderr("SHBT: Unknown exit action\n");
    _exit(EXIT_FAILURE);
  }
}

bool shbt_register_signal_handler(int sig_num, shbt_exit_action_t exit_action,
                                  void (*callback)(int)) {
  struct shbt_signal_info* sig_info = shbt_get_signal_info(sig_num);
  if (sig_info == NULL) {
    return false;
  }
  // Check for an environment variable overriding exit_action.
  char* env_exit_action = getenv("SHBT_SIGNAL_EXIT_ACTION");
  if (env_exit_action != NULL) {
    if (strncmp(env_exit_action, "EXIT", 4) == 0) {
      sig_info->exit_action = SHBT_EXIT_ACTION_EXIT;
    } else if (strncmp(env_exit_action, "RETURN", 6) == 0) {
      sig_info->exit_action = SHBT_EXIT_ACTION_RETURN;
    } else if (strncmp(env_exit_action, "RERAISE", 7) == 0) {
      sig_info->exit_action = SHBT_EXIT_ACTION_RERAISE;
    } else {
      return false;
    }
  } else {
    sig_info->exit_action = exit_action;
  }
#ifdef SHBT_HAVE_MPI
  // Set up MPI rank.
  if (mpi_rank == -1) {
    int init_flag;
    MPI_Initialized(&init_flag);
    if (init_flag) {
      MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    }
  }
#endif
  sig_info->callback = callback;
  // Set up the signal handler stack if needed.
  if (signal_handler_stack == NULL) {
    signal_handler_stack = malloc(SIGSTKSZ);
    if (signal_handler_stack == NULL) {
      return false;
    }
    stack_t ss;
    ss.ss_sp = signal_handler_stack;
    ss.ss_size = SIGSTKSZ;
    ss.ss_flags = 0;
    if (sigaltstack(&ss, NULL) < 0) {
      return false;
    }
  }
  struct sigaction sa;
  sa.sa_sigaction = &shbt_sigaction_handler;
  sigfillset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART | SA_SIGINFO | SA_ONSTACK;
  if (sigaction(sig_num, &sa, NULL) < 0) {
    return false;
  }
  return true;
}

bool shbt_register_signal_handlers(const int sig_nums[], size_t num_sig,
                                   shbt_exit_action_t exit_action,
                                   void (*callback)(int)) {
  for (size_t i = 0; i < num_sig; ++i) {
    if (!shbt_register_signal_handler(sig_nums[i], exit_action, callback)) {
      return false;
    }
  }
  return true;
}

bool shbt_register_fatal_handlers() {
  const int sig_nums[] = {
#ifdef SIGABRT
    SIGABRT,
#endif
#ifdef SIGALRM
    SIGALRM,
#endif
#ifdef SIGBUS
    SIGBUS,
#endif
#ifdef SIGEMT
    SIGEMT,
#endif
#ifdef SIGFPE
    SIGFPE,
#endif
#ifdef SIGHUP
    SIGHUP,
#endif
#ifdef SIGILL
    SIGILL,
#endif
#ifdef SIGINT
    SIGINT,
#endif
#ifdef SIGIO
    SIGIO,
#endif
#ifdef SIGLOST
    SIGLOST,
#endif
#ifdef SIGPIPE
    SIGPIPE,
#endif
#ifdef SIGPROF
    SIGPROF,
#endif
#ifdef SIGPWR
    SIGPWR,
#endif
#ifdef SIGQUIT
    SIGQUIT,
#endif
#ifdef SIGSEGV
    SIGSEGV,
#endif
#ifdef SIGSTKFLT
    SIGSTKFLT,
#endif
#ifdef SIGSYS
    SIGSYS,
#endif
#ifdef SIGTERM
    SIGTERM,
#endif
#ifdef SIGTRAP
    SIGTRAP,
#endif
#ifdef SIGUSR1
    SIGUSR1,
#endif
#ifdef SIGUSR2
    SIGUSR2,
#endif
#ifdef SIGVTALRM
    SIGVTALRM,
#endif
#ifdef SIGXCPU
    SIGXCPU,
#endif
#ifdef SIGXFSZ
    SIGXFSZ,
#endif
    0};
  return shbt_register_signal_handlers(
    sig_nums, (sizeof(sig_nums) / sizeof(int)) - 1,  // Ignore last 0.
    SHBT_EXIT_ACTION_EXIT, NULL);
}

bool shbt_register_signal_callback(int sig_num, void (*callback)(int)) {
  struct shbt_signal_info* sig_info = shbt_get_signal_info(sig_num);
  if (sig_info == NULL) {
    return false;
  }
  sig_info->callback = callback;
  return true;
}

bool shbt_register_signal_exit_action(int sig_num,
                                      shbt_exit_action_t exit_action) {
  struct shbt_signal_info* sig_info = shbt_get_signal_info(sig_num);
  if (sig_info == NULL) {
    return false;
  }
  sig_info->exit_action = exit_action;
  return true;
}

void __attribute__((destructor)) shbt_cleanup() {
  if (signal_handler_stack != NULL) {
    free(signal_handler_stack);
    signal_handler_stack = NULL;
  }
}
