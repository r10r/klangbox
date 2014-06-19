#include "module.h"

#define ARG_MATCH_PROP_KEY "property_name"

static const char* const g_argument_keys[] = { ARG_MATCH_PROP_KEY, NULL };

PA_MODULE_AUTHOR("Ruben Jenster");
PA_MODULE_DESCRIPTION(
	"Removes existing sinks with the same property=value"
	"as the added sink.");
PA_MODULE_VERSION("0.0.1");

struct userdata
{
	const char* match_property_name;
};

static bool
proplist_matches(pa_proplist* proplist, const char* key, const char* value)
{
	if (pa_proplist_contains(proplist, key))
	{
		const char* actual_value = pa_proplist_gets(proplist, key);
		return strcmp(actual_value, value) == 0;
	}
	return false;
}

static void
unload_duplicate_sinks(pa_core* core, pa_sink* sink, struct userdata* u)
{
	if (pa_proplist_contains(sink->proplist, u->match_property_name))
	{
		const char* expected_value = pa_proplist_gets(sink->proplist, u->match_property_name);

		void* state = NULL;
		pa_sink* other_sink = NULL;
		while ((other_sink = (pa_sink*)pa_idxset_iterate(core->sinks, &state, NULL)) != NULL)
		{
			if (other_sink != sink)
			{
				if (proplist_matches(other_sink->proplist, u->match_property_name, expected_value))
				{
					pa_log_notice("Unload sink #%u (%s)", other_sink->index, other_sink->name);
					pa_module_unload(core, other_sink->module, false);
				}
			}
		}
	}
}

int
pa__init(pa_module* m)
{
	pa_modargs* modargs = pa_modargs_new(m->argument, g_argument_keys);

	if (!modargs)
	{
		pa_log_error("Invalid or malformed arguments: %s", m->argument);
		return INIT_FAIL;
	}

	m->userdata = pa_xnew0(struct userdata, 1);
	struct userdata* u = (struct userdata*)m->userdata;

	u->match_property_name = pa_modargs_get_value(modargs, ARG_MATCH_PROP_KEY, NULL);
	if (!u->match_property_name)
	{
		pa_log_error("Argument[%s] value is required", ARG_MATCH_PROP_KEY);
		return INIT_FAIL;
	}

	pa_hook_connect(&m->core->hooks[PA_CORE_HOOK_SINK_PUT],
			PA_HOOK_EARLY, (pa_hook_cb_t)unload_duplicate_sinks, u);


	pa_log_notice("Initialized module %s : arguments[%s]", m->name, m->argument);
	return INIT_SUCCESS;
}

void
pa__done(pa_module* m)
{
	pa_log_notice("Destroying module %s", m->name);
	pa_xfree(m->userdata);
}
