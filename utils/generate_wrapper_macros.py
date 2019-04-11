#!/usr/bin/python
#
# Generate wrapper macros: hybris/include/hybris/internal/binding.h
#
# Usage:
# python utils/generate_wrapper_macros.py >hybris/include/hybris/internal/binding.h
#
# Copyright (C) 2013 Jolla Ltd.
# Contact: Thomas Perl <thomas.perl@jollamobile.com>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Modified by Shuyong Chen <shuyong.chen@gmail.com>

# Maximum number of arguments to generate wrapper macros for
MAX_ARGS = 20

BEGIN, END = '{', '}'

AUTO_GENERATED_WARNING = """
/**
 *         XXX AUTO-GENERATED FILE XXX
 *
 * Do not edit this file directly, but update the templates in
 * utils/generate_wrapper_macros.py and run it again to build
 * an updated version of this header file:
 *
 *    python utils/generate_wrapper_macros.py > \\
 *       hybris/include/hybris/internal/binding.h
 *
 * If you need macros with more arguments, just customize the
 * MAX_ARGS variable in generate_wrapper_macros.py.
 *
 *         XXX AUTO-GENERATED FILE XXX
 **/
"""

print """
/**
 * Copyright (C) 2013 Simon Busch <morphis@gravedo.de>
 *               2012 Canonical Ltd
 *               2013 Jolla Ltd.
 *
 * Auto-generated via "generate_wrapper_macros.py"
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/
/* Modified by Shuyong Chen <shuyong.chen@gmail.com> */

#ifndef HYBRIS_BINDING_H_
#define HYBRIS_BINDING_H_

/* floating_point_abi.h defines FP_ATTRIB */
#include <hybris/common/floating_point_abi.h>

#include "Message.h"
#include "Client.h"

extern void *android_dlopen (const char *filename, int flags);
extern int   android_dlclose(void *handle);
extern char *android_dlerror(void);
extern void *android_dlsym  (void *handle, const char *symbol);
"""

print AUTO_GENERATED_WARNING

print """
#define HYBRIS_DLSYSM(module, fptr, sym) \\
    if (!module##_handle) \\
        hybris_##module##_initialize(); \\
    if (*(fptr) == NULL) \\
    { \\
        *(fptr) = (void *) android_dlsym(module##_handle, sym); \\
    }

#define HYBRIS_LIBRARY_INITIALIZE(module, path) \\
    void *module##_handle; \\
    void hybris_##module##_initialize() \\
    { \\
        module##_handle = android_dlopen(path, RTLD_LAZY); \\
    }

#define HYBRIS_LIRBARY_CHECK_SYMBOL(module) \\
    int hybris_##module##_check_for_symbol(const char *sym) \\
    { \\
        return android_dlsym(module##_handle, sym) != NULL; \\
    }

"""

for count in range(MAX_ARGS):
    args = ['a%d' % (x+1) for x in range(count)]
    names = ['n%d' % (x+1) for x in range(count)]
    members_to_local = ['= message->n%d' % (x+1) for x in range(count)]
    local_to_members = ['message->n%d =' % (x+1) for x in range(count)]
    wrapper_signature = ', '.join(['module', 'return_type', 'symbol'] + args)
    signature = ', '.join(args)
    signature_with_names = ', '.join(' '.join(x) for x in zip(args, names))
    signature_with_members = '; '.join(' '.join(x) for x in zip(args, names))
    signature_with_members_to_local = '; '.join(' '.join(x) for x in zip(args, names, members_to_local))
    signature_with_local_to_members = '; '.join(' '.join(x) for x in zip(local_to_members, names))
    call_names = ', '.join(names)

    print """
#define HYBRIS_IMPLEMENT_FUNCTION{count}({wrapper_signature}) \\
    typedef struct Message_##symbol {BEGIN} \\
        Message common; \\
        {signature_with_members}; \\
        return_type result; \\
    {END} Message_##symbol; \\
    static return_type (*__dl_##symbol)({signature}) FP_ATTRIB = NULL; \\
    static void* handleMessage_##symbol (Message_##symbol* message) FP_ATTRIB; \\
    static void* handleMessage_##symbol (Message_##symbol* message) \\
    {BEGIN} \\
        {signature_with_members_to_local}; \\
        return_type result; \\
        result = __dl_##symbol({call_names}); \\
        message->result = result; \\
        return 0; \\
    {END} \\
    return_type symbol({signature_with_names}) \\
    {BEGIN} \\
        HYBRIS_DLSYSM(module, &__dl_##symbol, #symbol); \\
        MessageHandler handler[1]; \\
        Message_##symbol message[1]; \\
        handler->handleMessage = (void *(*) (Message* ))handleMessage_##symbol; \\
        {signature_with_local_to_members}; \\
        (void)MessageQueue_push_message(get_call_queue(), (Message* )message, handler); \\
        return message->result; \\
    {END}
""".format(**locals())

for count in range(MAX_ARGS):
    args = ['a%d' % (x+1) for x in range(count)]
    names = ['n%d' % (x+1) for x in range(count)]
    members_to_local = ['= message->n%d' % (x+1) for x in range(count)]
    local_to_members = ['message->n%d =' % (x+1) for x in range(count)]
    wrapper_signature = ', '.join(['module', 'symbol'] + args)
    signature = ', '.join(args)
    signature_with_names = ', '.join(' '.join(x) for x in zip(args, names))
    signature_with_members = '; '.join(' '.join(x) for x in zip(args, names))
    signature_with_members_to_local = '; '.join(' '.join(x) for x in zip(args, names, members_to_local))
    signature_with_local_to_members = '; '.join(' '.join(x) for x in zip(local_to_members, names))
    call_names = ', '.join(names)
    print """
#define HYBRIS_IMPLEMENT_VOID_FUNCTION{count}({wrapper_signature}) \\
    typedef struct Message_##symbol {BEGIN} \\
        Message common; \\
        {signature_with_members}; \\
    {END} Message_##symbol; \\
    static void (*__dl_##symbol)({signature}) FP_ATTRIB = NULL; \\
    static void* handleMessage_##symbol (Message_##symbol* message) FP_ATTRIB; \\
    static void* handleMessage_##symbol (Message_##symbol* message) \\
    {BEGIN} \\
        {signature_with_members_to_local}; \\
        __dl_##symbol({call_names}); \\
        return 0; \\
    {END} \\
    void symbol({signature_with_names}) \\
    {BEGIN} \\
        HYBRIS_DLSYSM(module, &__dl_##symbol, #symbol); \\
        MessageHandler handler[1]; \\
        Message_##symbol message[1]; \\
        handler->handleMessage = (void *(*) (Message* ))handleMessage_##symbol; \\
        {signature_with_local_to_members}; \\
        (void)MessageQueue_push_message(get_call_queue(), (Message* )message, handler); \\
    {END}
""".format(**locals())

# Print it again, so people wanting to append new macros will see it
print AUTO_GENERATED_WARNING

print """
#endif /* HYBRIS_BINDING_H_ */
"""

