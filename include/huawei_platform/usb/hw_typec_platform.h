

#ifndef __HW_TYPEC_PLATFORM_H__
#define __HW_TYPEC_PLATFORM_H__

/* detect type-c protocol current for external modules */
int typec_detect_input_current(void);

/* detect type-c inserted plug orientation for external modules */
int typec_detect_cc_orientation(void);

#endif
