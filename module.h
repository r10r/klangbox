#ifndef _KLANGBOX_MODULE_H_
#define _KLANGBOX_MODULE_H_

#include "../config.h"  /* generated from configure script */
#include "pulsecore/module.h"
#include "pulsecore/log.h"
#include "pulsecore/modargs.h"
#include "pulsecore/core.h"
#include "pulsecore/core-util.h"
#include "pulsecore/protocol-native.h"

#define INIT_SUCCESS 0
#define INIT_FAIL -1

/* only required for libpulse client API */

#define is_event_type(t, _type) \
	((t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) == _type)

#define is_event_operation(t, _operation) \
	((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == _operation)

#endif
