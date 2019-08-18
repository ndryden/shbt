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
 * C++ symbol demangling via builtin compiler ABI.
 *
 * WARNING: This is unsafe to call from a signal handler and should only be
 * used if absolutely needed.
 *
 * Note: This is a .cpp file since cxxabi.h is typically only in the default
 * include paths for C++ compilation.
 */

#include "shbt/shbt_internal.h"

#ifdef SHBT_USE_ABI_DEMANGLER

#include <stdlib.h>
#include <string.h>
#include <cxxabi.h>

bool shbt_demangle(const char* mangled, char* out, size_t out_size) {
  if (out_size == 0) {
    return false;
  }
  size_t abi_out_size;
  int abi_status;
  char* abi_out = abi::__cxa_demangle(mangled, NULL, &abi_out_size, &abi_status);
  if (abi_status != 0) {
    if (abi_out != NULL) {
      free(abi_out);
    }
    return false;
  }
  if (abi_out_size > out_size) {
    if (abi_out != NULL) {
      free(abi_out);
    }
    out[0] = '\0';
    return false;
  }
  strncpy(out, abi_out, abi_out_size);
  free(abi_out);
  return true;
}

#endif  // SHBT_USE_ABI_DEMANGLER
