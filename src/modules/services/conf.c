#include <cJSON.h>
#include "services.h"

void
conf_load(Module_Services *services)
{
   cJSON *json,
         *command,
         *start,
         *stop,
         *restart;

   DBG("Loading %s", MODULE_SERVICES_CONF);
   json = gotham_modules_conf_load(MODULE_SERVICES_CONF);
   EINA_SAFETY_ON_NULL_RETURN(json);

   command = cJSON_GetObjectItem(json, "commands");
   EINA_SAFETY_ON_NULL_GOTO(command, free_json);

   start = cJSON_GetObjectItem(command, "start");
   EINA_SAFETY_ON_NULL_GOTO(start, free_json);

   stop = cJSON_GetObjectItem(command, "stop");
   EINA_SAFETY_ON_NULL_GOTO(stop, free_json);

#ifndef _WIN32
   restart = cJSON_GetObjectItem(command, "restart");
   EINA_SAFETY_ON_NULL_GOTO(restart, free_json);
#endif

   services->commands.start = strdup(start->valuestring);
   EINA_SAFETY_ON_NULL_GOTO(services->commands.start, free_json);

   services->commands.stop = strdup(stop->valuestring);
   EINA_SAFETY_ON_NULL_GOTO(services->commands.stop, free_start);

#ifndef _WIN32
   services->commands.restart = strdup(restart->valuestring);
   EINA_SAFETY_ON_NULL_GOTO(services->commands.restart, free_stop);
#endif

   return;

free_stop:
   free((char *)services->commands.stop);
free_start:
   free((char *)services->commands.start);
free_json:
   cJSON_Delete(json);
}
