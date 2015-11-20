noinst_LTLIBRARIES = \
   src/lib/libcjson.la

src_lib_libcjson_la_SOURCES = \
   src/lib/extras/cJSON.c \
   src/include/cJSON.h
src_lib_libcjson_la_LDFLAGS = -lm
