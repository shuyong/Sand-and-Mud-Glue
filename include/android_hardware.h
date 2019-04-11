
#ifndef _ANDROID_HARDWARE_H_
#define _ANDROID_HARDWARE_H_

#include <hardware/hardware.h>

/*
 * Attribute for softfp-calling-style functions
 */
#ifdef __ARM_PCS_VFP
#    define SOFTFP __attribute__((pcs("aapcs")))
#else
#    define SOFTFP
#endif

__BEGIN_DECLS

extern int (*__dl_hw_get_module)(const char *id, const struct hw_module_t **module) SOFTFP;
extern int (*__dl_hw_get_module_by_class)(const char *class_id, const char *inst,
                           const struct hw_module_t **module) SOFTFP;

extern void android_hardware_initialize();

extern int hw_get_module_r(const char *id, const struct hw_module_t **module);
extern int hw_get_module_by_class_r(const char *class_id, const char *inst,
                           const struct hw_module_t **module);

__END_DECLS

#endif /* _ANDROID_HARDWARE_H_ */
