/*
 * Copyright (c) 2012 Simon Busch <morphis@gravedo.de>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <dlfcn.h>
#include <stddef.h>
#include <errno.h>
#include <hardware/hardware.h>
#include <hybris/common/binding.h>

#include "android_hardware.h"

#define HARDWARE_DLSYSM(sym) HYBRIS_DLSYSM(hardware, &__dl_##sym, #sym)

#pragma GCC visibility push(hidden)
HYBRIS_LIBRARY_INITIALIZE(hardware, "libhardware.so");
#if 1
HYBRIS_LIRBARY_CHECK_SYMBOL(hardware);
#endif
#pragma GCC visibility pop

int (*__dl_hw_get_module)(const char *id, const struct hw_module_t **module) SOFTFP = NULL;
int (*__dl_hw_get_module_by_class)(const char *class_id, const char *inst,
                           const struct hw_module_t **module) SOFTFP = NULL;

void android_hardware_initialize()
{
    HARDWARE_DLSYSM(hw_get_module);
    HARDWARE_DLSYSM(hw_get_module_by_class);
}

static int __dl_hw_get_module_r(const char *id, const struct hw_module_t **module) SOFTFP;
static int __dl_hw_get_module_r(const char *id, const struct hw_module_t **module) {
    return __dl_hw_get_module(id, module);
}

static int __dl_hw_get_module_by_class_r(const char *class_id, const char *inst,
                           const struct hw_module_t **module) SOFTFP;
static int __dl_hw_get_module_by_class_r(const char *class_id, const char *inst,
                           const struct hw_module_t **module) {
    return __dl_hw_get_module_by_class(class_id, inst, module);
}

ANDROID_IMPLEMENT2(int, hw_get_module_r, const char *, const struct hw_module_t **);
ANDROID_IMPLEMENT3(int, hw_get_module_by_class_r, const char *, const char *, const struct hw_module_t **);

// vim:ts=4:sw=4:noexpandtab
