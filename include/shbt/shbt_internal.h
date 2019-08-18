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

/**
 * Internal function declarations, do not include directly.
 */

#pragma once

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500  // In case this wasn't already defined.
#endif
#include <signal.h>
#include <stdint.h>
#include <stdbool.h>

#include "shbt/shbt.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Internal information on signals. */
struct shbt_signal_info {
  /** Signal number. */
  int sig_num;
  /** Name of the signal. */
  const char* sig_name;
  /** Description of the signal. */
  const char* sig_desc;
  /** Action to take on exit. */
  shbt_exit_action_t exit_action;
  /** Callback to invoke. */
  void (*callback)(int);
};
/** Internal information on signal codes. */
struct shbt_signal_code_info {
  /** Code number. */
  int code_num;
  /** Code name. */
  const char* code_name;
  /** Code description. */
  const char* code_desc;
};

/**
 * Print to file descriptor.
 *
 * This is safe to call from a signal handler.
 *
 * @param output String to print. Must be null-terminated.
 * @param fd File descriptor to print to.
 */
void shbt_safe_print(const char* output, int fd);
/**
 * Print to stderr.
 *
 * This is safe to call from a signal handler.
 *
 * @param output String to print. Must be null-terminated.
 */
void shbt_print_to_stderr(const char* output);

/**
 * Return signal information struct.
 *
 * Returns NULL if signal information is not found.
 *
 * This is safe to call from a signal handler.
 *
 * @param sig_num The signal number.
 */
struct shbt_signal_info* shbt_get_signal_info(int sig_num);
/**
 * Return signal code information struct.
 *
 * Returns NULL if signal code information is not found.
 *
 * This is safe to call from a signal handler.
 *
 * @param info_list List of signal code information for the signal
 * associated with this code.
 * @param code_num Signal code.
 */
const struct shbt_signal_code_info* shbt_get_signal_code_info(
  const struct shbt_signal_code_info info_list[], int code_num);

/**
 * Print detailed signal information to stderr
 *
 * This is safe to call from a signal handler.
 *
 * @param sig_num The signal number.
 * @param info Additional signal information.
 */
void shbt_print_signal(int sig_num, siginfo_t* info);
/**
 * Actual signal handler.
 */
void shbt_sigaction_handler(int sig_num, siginfo_t* info, void* void_ucontext);

/**
 * Convert an integer to a string.
 *
 * This writes a string representation of the integer i to buf. If there is
 * insufficient space to write it, NULL is returned. Otherwise, buf is
 * returned.
 *
 * Negative numbers are supported only for base 10. Bases 2 through 16 are
 * supported.
 *
 * This is safe to call from a signal handler.
 *
 * Note this is essentially a reimplementation the (non-standard) itoa.
 *
 * @param i Integer to get string representation for.
 * @param buf Buffer to write to.
 * @param size Size of buf.
 * @param base Desired base for the representation.
 * @param pad Prepend up to this many 0s.
 */
char* shbt_itoa(intptr_t i, char* buf, size_t size, int base, size_t pad);

/**
 * Demangle a mangled symbol from the Itanium C++ ABI.
 *
 * Returns true if demangling is successful, false otherwise.
 *
 * This is safe to call from a signal handler unless SHBT_USE_ABI_DEMANGLER
 * is defined.
 *
 * @param mangled The symbol to demangle.
 * @param out Output buffer.
 * @param out_size Size of the output buffer.
 */
bool shbt_demangle(const char* mangled, char* out, size_t out_size);

/**
 * Handle internal cleanup on exit.
 */
void __attribute__((destructor)) shbt_cleanup();

#ifdef __cplusplus
}  // extern "C"
#endif
