#ifdef _WIN32
#include "services.h"

BOOL
_command_win32_wait_state(SC_HANDLE service,
                          DWORD state)
{
   SERVICE_STATUS_PROCESS ssStatus;
   DWORD dwOldCheckPoint;
   DWORD dwStartTickCount;
   DWORD dwWaitTime;
   DWORD dwBytesNeeded;
   BOOL r;

   dwStartTickCount = GetTickCount();
   dwOldCheckPoint = ssStatus.dwCheckPoint;

begin:
   r = QueryServiceStatusEx(service, SC_STATUS_PROCESS_INFO,
                            (LPBYTE) &ssStatus, sizeof(SERVICE_STATUS_PROCESS),
                            &dwBytesNeeded);
   if (!r)
     {
        ERR("QueryServiceStatusEx() failed (%d)", GetLastError());
        return FALSE;
     }

   if (ssStatus.dwCurrentState != state)
     return TRUE;

   dwWaitTime = ssStatus.dwWaitHint / 10;
   if (dwWaitTime < 1000)
     dwWaitTime = 1000;
   else if (dwWaitTime > 10000)
     dwWaitTime = 10000;

   Sleep(dwWaitTime);

   if (ssStatus.dwCheckPoint > dwOldCheckPoint)
     {
        dwStartTickCount = GetTickCount();
        dwOldCheckPoint = ssStatus.dwCheckPoint;
     }
   else if (GetTickCount()-dwStartTickCount > ssStatus.dwWaitHint)
     {
        ERR("Timed out waiting for given state");
        return FALSE;
     }

   goto begin;
}

#define _ERR(_a, _b, ...)                                                      \
   do {                                                                        \
      if (!(_a)) break;                                                        \
      ERR(__VA_ARGS__);                                                        \
      goto _b;                                                                 \
   } while (0)

BOOL
_command_win32_init(SC_HANDLE *manager,
                    SC_HANDLE *service,
                    SERVICE_STATUS_PROCESS *status,
                    const char *name)
{
   BOOL r;
   DWORD len;

   *manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
   _ERR(!*manager, error, "OpenSCManager failed (%d)", GetLastError());

   *service = OpenService(*manager, name, SERVICE_ALL_ACCESS);
   _ERR(!*service, close_manager, "OpenService failed (%d)", GetLastError());

   r = QueryServiceStatusEx(*service, SC_STATUS_PROCESS_INFO, (LPBYTE) status,
                            sizeof(SERVICE_STATUS_PROCESS), &len);
   _ERR(!r, close_service, "Failed to query service");

   return TRUE;

close_service:
   CloseServiceHandle(*service);
close_manager:
   CloseServiceHandle(*manager);
error:
   return FALSE;
}

#undef _ERR

#define _ERR(_a, _b, _c, _d)                                                   \
   do {                                                                        \
      if (!(_a))                                                               \
        break;                                                                 \
      if (_b->citizen->type == GOTHAM_CITIZEN_TYPE_ALFRED)                     \
        module_json_answer(".service", _d, EINA_FALSE, _c, _b->citizen->gotham,\
                           _b->citizen->jid);                                  \
      else gotham_command_send(_b, _c);                                        \
      goto close_service;                                                      \
   } while (0)
/* "debug" */

Eina_Bool
command_win32_start(Module_Services *services,
                    Gotham_Citizen_Command *command,
                    const char *name)
{
   SC_HANDLE manager,
             service;
   SERVICE_STATUS_PROCESS status;
   BOOL r;

   DBG("services[%p] command[%p] name[%s]", services, command, name);

   r = _command_win32_init(&manager, &service, &status, name);
   _ERR(!r, command, "Failed to access service", "start");

   if ((status.dwCurrentState != SERVICE_STOPPED) &&
       (status.dwCurrentState != SERVICE_STOP_PENDING))
     goto end;

   r = _command_win32_wait_state(service, SERVICE_STOP_PENDING);
   _ERR(!r, command, "Service stuck in a stop state", "start");

   r = StartService(service, 0, NULL);
   _ERR(!r, command, "Failed to start service", "start");

   r = _command_win32_wait_state(service, SERVICE_START_PENDING);
   _ERR(!r, command, "Service stuck in a start state", "start");

end:
   CloseServiceHandle(service);
   CloseServiceHandle(manager);
   return EINA_TRUE;

close_service:
   CloseServiceHandle(service);
   return EINA_FALSE;
}

Eina_Bool
command_win32_stop(Module_Services *services,
                   Gotham_Citizen_Command *command,
                   const char *name)
{
   SC_HANDLE manager,
             service;
   SERVICE_STATUS_PROCESS status;
   BOOL r;

   DBG("services[%p] command[%p] name[%s]", services, command, name);

   r = _command_win32_init(&manager, &service, &status, name);
   _ERR(!r, command, "Failed to access service", "stop");

   _ERR(status.dwCurrentState == SERVICE_STOPPED, command,
        "Service is already stopped", "stop");

   r = _command_win32_wait_state(service, SERVICE_STOP_PENDING);
   _ERR(!r, command, "Service stuck in a stop state", "stop");

   r = ControlService(service, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS) &status);
   _ERR(!r, command, "Failed to stop service", "stop");

   CloseServiceHandle(service);
   CloseServiceHandle(manager);
   return EINA_TRUE;;

close_service:
   CloseServiceHandle(service);
   return EINA_FALSE;
}

#undef _ERR
#endif
