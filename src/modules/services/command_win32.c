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

void
command_win32_start(Module_Services *services,
                    Gotham_Citizen_Command *command,
                    const char *name)
{
   SERVICE_STATUS_PROCESS ssStatus;
   DWORD dwBytesNeeded;
   BOOL r;
   SC_HANDLE manager,
             service;

#define _ERR(_a, _b, _c)                                                       \
   do {                                                                        \
      if (!(_a))                                                               \
        break;                                                                 \
      gotham_command_send(_b, _c);                                             \
      goto close_service;                                                      \
   } while (0)

   DBG("services[%p] command[%p] name[%s]", services, command, name);

   manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
   _ERR(!manager, command, "Failed to open service manager");

   service = OpenService(manager, name, SERVICE_ALL_ACCESS);
   _ERR(!service, command, "Failed to open service");

   r = QueryServiceStatusEx(service, SC_STATUS_PROCESS_INFO,
                            (LPBYTE) &ssStatus, sizeof(SERVICE_STATUS_PROCESS),
                            &dwBytesNeeded);
   _ERR(!r, command, "Failed to query service");

   _ERR((ssStatus.dwCurrentState != SERVICE_STOPPED) &&
        (ssStatus.dwCurrentState != SERVICE_STOP_PENDING),
        command, "Service already started");

   r = _command_win32_wait_state(service, SERVICE_STOP_PENDING);
   _ERR(!r, command, "Service stuck in a stop state");

   r = StartService(service, 0, NULL);
   _ERR(!r, command, "Failed to start service");

   r = _command_win32_wait_state(service, SERVICE_START_PENDING);
   _ERR(!r, command, "Service stuck in a start state");

#undef _ERR

   gotham_command_send(command, "Service started");
   return;

close_service:
   CloseServiceHandle(service);
close_manager:
   CloseServiceHandle(manager);
}
#endif
