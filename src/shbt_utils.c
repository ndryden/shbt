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
 *
 * This file contains code adapted from the Chromium project. See below
 * for details, and LICENSE for more information.
 */

#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include "shbt/shbt.h"
#include "shbt/shbt_internal.h"

void shbt_safe_print(const char* output, int fd) {
  ssize_t r;
  do {
    r = write(fd, output, strlen(output));
  } while (r == -1 && errno == EINTR);
}

void shbt_print_to_stderr(const char* output) {
  shbt_safe_print(output, STDERR_FILENO);
}

// Implementation adapted from Chromium base/debug/stack_trace_posix.cc.
// See LICENSE for more information.
char* shbt_itoa(intptr_t i, char* buf, size_t size, int base, size_t pad) {
  // Ensure we can write at least one NULL byte.
  if (size == 0) {
    return NULL;
  }

  if (base < 2 || base > 16) {
    buf[0] = '\000';
    return NULL;
  }

  size_t n = 1;
  char* start = buf;
  uintptr_t j = i;

  // Handle negative numbers for base 10 only.
  if (i < 0 && base == 10) {
    // Negate i while avoiding overflow.
    j = ((uintptr_t) (-(i + 1))) + 1;
    // Ensure we have enough space to write the '-'.
    if (++n > size) {
      buf[0] = '\000';
      return NULL;
    }
    *start++ = '-';
  }

  // Convert the entire number, outputting at least one character (0).
  char* ptr = start;
  do {
    // Ensure we have enough space.
    if (++n > size) {
      buf[0] = '\000';
      return NULL;
    }
    // Output the next digit.
    *ptr++ = "0123456789abcdef"[j % base];
    j /= base;
    // Handle padding.
    if (pad > 0) {
      --pad;
    }
  } while (j > 0 || pad > 0);

  // Add terminating null character.
  *ptr = '\000';

  // The digits are currently in reverse order. This reverses the string,
  // except for any initial '-', which is in the right place.
  while (--ptr > start) {
    char ch = *ptr;
    *ptr = *start;
    *start++ = ch;
  }
  return buf;
}
