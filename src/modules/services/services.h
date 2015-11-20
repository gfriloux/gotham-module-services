#ifndef SERVICES_H
# define SERVICES_H

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include <Eina.h>
# include <Ecore.h>
# include <Gotham.h>

#define MODULE_SERVICES_CONF SYSCONF_DIR"/gotham/modules.conf.d/services.conf"

int _module_services_log_dom;

typedef struct _Module_Services
{
   Gotham *gotham;

   struct
   {
      const char *start,
#ifndef _WIN32
                 *restart,
#endif
                 *stop;
   } commands;
} Module_Services;


#define CRI(...) EINA_LOG_DOM_CRIT(_module_services_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_module_services_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_module_services_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_module_services_log_dom, __VA_ARGS__)

void conf_load(Module_Services *services);

void command_start(Module_Services *services, Gotham_Citizen_Command *command);
#ifndef _WIN32
void command_restart(Module_Services *services, Gotham_Citizen_Command *command);
#endif
void command_stop(Module_Services *services, Gotham_Citizen_Command *command);

char * utils_strdupf(const char *s, ...);
#endif
