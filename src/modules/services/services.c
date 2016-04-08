#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>

#include "services.h"

static const char *name = "Services";

void *
module_register(Gotham *gotham)
{
   Module_Services *services;

   if (gotham->me->type != GOTHAM_CITIZEN_TYPE_BOTMAN)
     return NULL;

   services = calloc(1, sizeof(Module_Services));
   EINA_SAFETY_ON_NULL_RETURN_VAL(services, NULL);

   DBG("services[%p] gotham[%p] shotgun[%p]",
       services, gotham, gotham->shotgun);

   services->gotham = gotham;

   gotham_modules_command_add("services", ".service start",
                              "[.service start servicename] - "
                              "This command will start a given service.");
   gotham_modules_command_add("services", ".service stop",
                              "[.service stop servicename] - "
                              "This command will stop a given service.");
   gotham_modules_command_add("services", ".service restart",
                              "[.service restart servicename] - "
                              "This command will restart a given service.");
#ifndef _WIN32
   conf_load(services);
#endif
   return services;
}

void
module_unregister(void *data)
{
   Module_Services *services = data;

   DBG("services[%p]", services);

   free(services);
}

const char *
module_init(void)
{
   eina_init();
   _module_services_log_dom = eina_log_domain_register("module_services",
                                                       EINA_COLOR_RED);
   return name;
}

void
module_shutdown(void)
{
   eina_log_domain_unregister(_module_services_log_dom);
   eina_shutdown();
}

void
module_json_answer(const char *cmd,
                   const char *params,
                   Eina_Bool status,
                   const char *content,
                   Gotham *gotham,
                   const char *jid)
{
   cJSON *json_obj,
         *json_content;
   char **split;
   unsigned int i;
   char *ptr;

   json_obj = cJSON_CreateObject();
   cJSON_AddStringToObject(json_obj, "command", cmd);
   cJSON_AddStringToObject(json_obj, "parameters", params);
   cJSON_AddStringToObject(json_obj, "status", status ? "ok" : "ko");

   json_content = cJSON_CreateArray();
   split = eina_str_split(content, "\n", 0);
   for (i=0; split[i]; i++)
     if (split[i][0])
       cJSON_AddItemToArray(json_content, cJSON_CreateString(split[i]));

   cJSON_AddItemToObject(json_obj, "content", json_content);
   if (split[0]) free(split[0]);
   free(split);

   ptr = cJSON_Print(json_obj);
   cJSON_Delete(json_obj);
   shotgun_message_send(gotham->shotgun, jid, ptr,
                        SHOTGUN_MESSAGE_STATUS_ACTIVE, EINA_TRUE);

   free(ptr);
   return;
}
