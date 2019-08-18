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

#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define UNW_LOCAL_ONLY  // Only need the local API for libunwind.
#include <libunwind.h>

#include "shbt/shbt.h"
#include "shbt/shbt_internal.h"

bool shbt_collect_backtrace(shbt_frame_t trace[], size_t num_frames,
                            size_t* num_valid_frames) {
  unw_context_t context;
  unw_getcontext(&context);
  unw_cursor_t cursor;
  unw_init_local(&cursor, &context);
  size_t cur_frame = 0;
  const char* unknown_symbol_str = "(unknown symbol)";
  while (cur_frame < num_frames && unw_step(&cursor) > 0) {
    unw_word_t offp;
    if (unw_get_proc_name(&cursor, trace[cur_frame].symbol,
                          sizeof(trace[cur_frame].symbol), &offp)) {
      // Failed to get symbol name.
      strncpy(trace[cur_frame].symbol, unknown_symbol_str,
              sizeof(trace[cur_frame].symbol));
    }
    ++cur_frame;
  }
  *num_valid_frames = cur_frame;
  return true;
}

bool shbt_print_collected_backtrace_fd(shbt_frame_t trace[], size_t num_frames,
                                       int fd) {
  char str_buf[128] = {0};  // Should be sufficiently large.
  char demangled_symbol[1024] = {0};
  for (size_t cur_frame = 0; cur_frame < num_frames; ++cur_frame) {
    // Print frame number, with manual padding.
    if (cur_frame < 10) {
      shbt_safe_print("   ", fd);
    } else if (cur_frame < 100) {
      shbt_safe_print("  ", fd);
    } else if (cur_frame < 1000) {
      shbt_safe_print(" ", fd);
    }
    shbt_itoa(cur_frame, str_buf, sizeof(str_buf), 10, 0);
    shbt_safe_print(str_buf, fd);
    shbt_safe_print(": ", fd);
    if (shbt_demangle(trace[cur_frame].symbol, demangled_symbol,
                      sizeof(demangled_symbol))) {
      shbt_safe_print(demangled_symbol, fd);
#ifdef SHBT_USE_BUILTIN_IA64_DEMANGLER
      // Print the mangled symbol too, since this demangler doesn't fully
      // demangle some C++ stuff (function/template arguments, etc.).
      shbt_safe_print(" (", fd);
      shbt_safe_print(trace[cur_frame].symbol, fd);
      shbt_safe_print(")", fd);
#endif
    } else {
      shbt_safe_print(trace[cur_frame].symbol, fd);
    }
    shbt_safe_print("\n", fd);
  }
  return true;
}

bool shbt_print_backtrace_fd(int fd) {
  size_t depth = shbt_get_stack_depth();
  shbt_frame_t trace[depth];
  size_t valid_depth = 0;
  if (!shbt_collect_backtrace(trace, depth, &valid_depth)) {
    return false;
  }
  if (!shbt_print_collected_backtrace_fd(trace, valid_depth, fd)) {
    return false;
  }
  return true;
}

size_t shbt_get_stack_depth() {
  unw_context_t context;
  unw_getcontext(&context);
  unw_cursor_t cursor;
  unw_init_local(&cursor, &context);
  size_t cur_frame = 0;
  for (; unw_step(&cursor) > 0; ++cur_frame);
  return cur_frame;
}
