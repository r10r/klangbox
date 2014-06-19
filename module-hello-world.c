#include "../config.h"	/* generated from configure script */
#include "pulsecore/module.h"
#include "pulsecore/log.h"
#include "pulsecore/modargs.h"
#include "pulsecore/core.h"
#include "pulsecore/core-util.h"

static const char* const g_argument_keys[] = {"foo", "bar", NULL};

#define INIT_SUCCESS 0
#define INIT_FAIL -1

PA_MODULE_AUTHOR("Ruben Jenster");
PA_MODULE_DESCRIPTION("Simple hello world module");
PA_MODULE_LOAD_ONCE(1);
PA_MODULE_VERSION("0.0.1");

int
pa__init(pa_module* m)
{
	// TODO store parsed arguments in userdata ?
	pa_modargs* modargs = pa_modargs_new(m->argument, g_argument_keys);
	if (!modargs)
	{
		pa_log_error("Invalid or malformed arguments: %s", m->argument);
		return INIT_FAIL;
	}

	// to check whether pulse is running in system mode
	if (pa_in_system_mode())
	{
		pa_log_notice("PA really is indeed in system mode");
	}

	// entry point to the core API is m->core
//	m->core;

	pa_log_notice("Initialized module %s : arguments[%s]", m->name, m->argument);
  return INIT_SUCCESS;
}

void
pa__done(pa_module* m)
{
	pa_log_notice("Destroyed module %s", m->name);
}
