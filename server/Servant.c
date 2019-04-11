
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "Message.h"

void *Dlhook_handler (Message* msg) SOFTFP;
void *Dlhook_handler (Message* msg)
{
    MessageDlhook *message = (MessageDlhook *)msg;

    message->dlopen  = dlopen;
    message->dlclose = dlclose;
    message->dlerror = dlerror;
    message->dlsym   = dlsym;

    return message;
}

