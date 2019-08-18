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
 * Examine symbol demanging outputs.
 *
 * Note: Demanging is a potentially dangerous operation. This is just a simple
 * example/wrapper for this, and is not meant for anything serious.
 */

#include <stdlib.h>
#include <stdio.h>

#include "shbt/shbt.h"
// Only for testing purposes.
#include "shbt/shbt_internal.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Expected symbol to demangle\n");
    return 1;
  }

  char demangled[4096] = {0};  // Should be large enough.
  if (!shbt_demangle(argv[1], demangled, sizeof(demangled))) {
    printf("Error in demangling\n");
  }
  printf("Demangling result: %s\n", demangled);
  return 0;
}
