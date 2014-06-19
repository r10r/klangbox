#include "../config.h"
#include "pulsecore/module.h"
#include "pulsecore/log.h"
#include "pulsecore/modargs.h"

static const char** g_argument_keys = {"foo", "bar"};

#define INIT_SUCCESS 0
#define INIT_FAIL -1

int pa__init(pa_module* m)
{
	pa_modargs* modargs = pa_modargs_new(m->argument, g_argument_keys);
	if (!modargs)
	{
		pa_log_error("Invalid or malformed arguments: %s", m->argument);
		return INIT_FAIL;
	}

	pa_log_notice("Initialized module %s : arguments[%s]", m->name, m->argument);
  return INIT_SUCCESS;
}
