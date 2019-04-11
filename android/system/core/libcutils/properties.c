
#include <dlfcn.h>
#include <stddef.h>
#include <errno.h>
#include <cutils/properties.h>
#include <hybris/common/binding.h>

#pragma GCC visibility push(hidden)
HYBRIS_LIBRARY_INITIALIZE(cutils, "libcutils.so");
#if 1
HYBRIS_LIRBARY_CHECK_SYMBOL(cutils);
#endif
#pragma GCC visibility pop

HYBRIS_IMPLEMENT_FUNCTION3(cutils, int, property_get, const char *, char *, const char *);
HYBRIS_IMPLEMENT_FUNCTION2(cutils, int, property_set, const char *, const char *);
typedef void (*propfn_t)(const char *key, const char *value, void *cookie);
HYBRIS_IMPLEMENT_FUNCTION2(cutils, int, property_list, propfn_t, void *);

// vim:ts=4:sw=4:noexpandtab
