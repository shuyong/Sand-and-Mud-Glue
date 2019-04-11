#include <assert.h>
#include <stdio.h>
#include <sys/types.h>

#include "Message.h"
#include "Client.h"

void *(*__dl_dlopen) (const char *filename, int flags) = NULL;
int   (*__dl_dlclose)(void *handle) = NULL;
char *(*__dl_dlerror)(void) = NULL;
void *(*__dl_dlsym)  (void *handle, const char *symbol) = NULL;

static MessageQueue *dlhook_queue;
static MessageQueue *call_queue;

int Client_initialize (struct io_port* port1, struct io_port* port2)
{
    dlhook_queue = MessageQueue_ctor(port1->rfd, port1->wfd);
    call_queue = MessageQueue_ctor(port2->rfd, port2->wfd);

    return 0;
}

MessageQueue *get_dlhook_queue ()
{
    return dlhook_queue;
}

MessageQueue *get_call_queue ()
{
    return call_queue;
}

static void *handleDummy (Message* message) SOFTFP;
static void *handleDummy (Message* message) 
{
    return message;
}

void  dlhook_initialize ()
{
    if (__dl_dlopen != NULL) return;

    MessageHandler handler[1];
    MessageDlhook MessageDlhook[1];
    Message* message = (Message* )MessageDlhook;

    handler->handleMessage = (void *(*) (Message* ) )handleDummy;
    message->what = MSG_DLHOOK;
    MessageDlhook->dlopen  = NULL;
    MessageDlhook->dlclose = NULL;
    MessageDlhook->dlerror = NULL;
    MessageDlhook->dlsym   = NULL;

    (void)MessageQueue_push_message(get_dlhook_queue(), (Message* )message, handler);

    __dl_dlopen  = MessageDlhook->dlopen ;
    __dl_dlclose = MessageDlhook->dlclose;
    __dl_dlerror = MessageDlhook->dlerror;
    __dl_dlsym   = MessageDlhook->dlsym  ;
}

