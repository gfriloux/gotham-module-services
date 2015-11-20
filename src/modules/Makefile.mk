MAINTAINERCLEANFILES += \
src/lib/modules/services/*.gc{no,da}

moddir = $(libdir)/gotham/$(MODULE_ARCH)
mod_LTLIBRARIES = \
   src/modules/services.la

src_modules_services_la_SOURCES = \
   src/modules/services/services.c \
   src/modules/services/services.h \
   src/modules/services/command.c \
   src/modules/services/command_win32.c \
   src/modules/services/event.c \
   src/modules/services/conf.c \
   src/modules/services/utils.c
src_modules_services_la_CFLAGS = \
   $(SERVICES_CFLAGS) -DDATA_DIR=\"$(datadir)\" \
   -DSYSCONF_DIR=\"$(sysconfdir)\"
src_modules_services_la_LDFLAGS = \
   `pkg-config --libs $(SERVICES_REQUIRES)` \
   -no-undefined -module -avoid-version \
   -lm
src_modules_services_la_LIBADD = \
   src/lib/libcjson.la
src_modules_services_la_LIBTOOLFLAGS = --tag=disable-static

