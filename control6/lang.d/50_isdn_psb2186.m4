################################
# ISDN PSB2186 Call Monitoring
################################
define(`ISDN_PSB2186_USED', `ifdef(`isdn_psb2186_used', `', `dnl
define(`old_divert', divnum)dnl
define(`isdn_psb2186_used')dnl
divert(globals_divert)
#ifndef PSB2186_SUPPORT
#error Please define ISDN PSB2186 support
#endif

#include "hardware/isdn/psb2186.h"
extern psb2186_callback_t psb2186_callback;
psb2186_callback_t isdn_psb2186_nextcallback = NULL;

uint8_t isdn_psb2186_newcall = 0;

char isdn_psb2186_callback_callee[CALLEE_BUF_LENGTH];
uint8_t isdn_psb2186_callback_callee_len;
char* isdn_psb2186_callee;
char isdn_psb2186_callback_caller[CALLER_BUF_LENGTH];
uint8_t isdn_psb2186_callback_caller_len;
char* isdn_psb2186_caller;

void isdn_psb2186_callback(char *callee, char *caller) {
  isdn_psb2186_newcall = 1;
  isdn_psb2186_callback_callee_len = strlen(callee);
  strncpy(isdn_psb2186_callback_callee, callee, CALLEE_BUF_LENGTH);
  isdn_psb2186_callback_caller_len = strlen(caller);
  strncpy(isdn_psb2186_callback_caller, caller, CALLER_BUF_LENGTH);
  if(isdn_psb2186_nextcallback != NULL)
    isdn_psb2186_nextcallback(callee, caller);
}

uint8_t isdn_if_call(void) {
  if(isdn_psb2186_newcall) {
    isdn_psb2186_newcall = 0;
    return 1;
  }
  return 0;
}

define(`CALLEE', `isdn_psb2186_callback_callee')
define(`CALLEE_LEN', `isdn_psb2186_callback_callee_len')
define(`CALLER', `isdn_psb2186_callback_caller')
define(`CALLER_LEN', `isdn_psb2186_callback_caller_len')

divert(init_divert)
  if(psb2186_callback != NULL)
    isdn_psb2186_nextcallback = psb2186_callback;
  psb2186_callback = isdn_psb2186_callback;


divert(old_divert)')')
define(`IFCALL', `ISDN_PSB2186_USED()isdn_if_call()')

define(`WAITCALL', `INTHREAD(`', `DIE(`Can use WAITCALL only in a THREAD')')ISDN_PSB2186_USED()
`isdn_psb2186_callback_blocking'action_thread_ident` = 1;
if(psb2186_callback != NULL)
  isdn_psb2186_nextcallback'action_thread_ident` = psb2186_callback;
psb2186_callback = isdn_psb2186_callback'action_thread_ident`;
PT_WAIT_WHILE(pt, isdn_psb2186_callback_blocking'action_thread_ident` == 1);
psb2186_callback = isdn_psb2186_nextcallback'action_thread_ident`;
isdn_psb2186_nextcallback'action_thread_ident` = NULL;'
ifdef(`isdn_psb2186_callback_defined'action_thread_ident, `', `
define(`isdn_psb2186_callback_defined'action_thread_ident, 1)
define(`old_divert', divnum)dnl
divert(globals_divert)dnl
uint8_t isdn_psb2186_callback_blocking'action_thread_ident`;

psb2186_callback_t isdn_psb2186_nextcallback'action_thread_ident` = NULL;

void isdn_psb2186_callback'action_thread_ident`(char *callee, char *caller) {
  isdn_psb2186_callback_blocking'action_thread_ident` = 0;
  if(isdn_psb2186_nextcallback'action_thread_ident` != NULL)
    isdn_psb2186_nextcallback'action_thread_ident`(callee, caller);
}')

divert(old_divert)')')

