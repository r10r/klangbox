#include "module.h"

#define ARG_MATCH_PROP_KEY "property_name"

static const char* const g_argument_keys[] = { ARG_MATCH_PROP_KEY, NULL};

PA_MODULE_AUTHOR("Ruben Jenster");
PA_MODULE_DESCRIPTION(
		"Couples an input with a sink."
		"Sink and input are coupled by a property=key."
		"When input and sink are coupled, then: "
		"* a new input is moved to an existing sink."
		"* an existing input is moved to a new sink.");
PA_MODULE_LOAD_ONCE(1);
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
		return (strcmp(actual_value, value) == 0);
	}
	return false;
}

static void
put_input_to_sink(pa_core *core, pa_sink_input *input, struct userdata *u)
{
	if(pa_proplist_contains(input->proplist, u->match_property_name))
	{
		const char* expected_value = pa_proplist_gets(input->proplist, u->match_property_name);
		void *state = NULL;
		pa_sink* sink = NULL;
		while ((sink = (pa_sink*)pa_idxset_iterate(core->sinks, &state, NULL)) != NULL)
		{
			if (proplist_matches(sink->proplist, u->match_property_name, expected_value))
			{
				pa_log_notice("Move new input #%d to existing sink [%s]", input->index, sink->name);
				pa_sink_input_move_to(input, sink, false);
				break;
			}
		}
	}
}

static void
move_input_to_sink(pa_core *core, pa_sink* sink, struct userdata *u)
{
	if(pa_proplist_contains(sink->proplist, u->match_property_name))
	{
		const char* expected_value = pa_proplist_gets(sink->proplist, u->match_property_name);

		void *state = NULL;
		pa_sink_input* input = NULL;
		while ((input = (pa_sink_input*)pa_idxset_iterate(core->sink_inputs, &state, NULL)) != NULL)
		{
			if (proplist_matches(input->proplist, u->match_property_name, expected_value))
			{
				pa_log_notice("Move existing input #%d to new sink [%s]", input->index, sink->name);
				pa_sink_input_move_to(input, sink, false);
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
	struct userdata *u = (struct userdata*)m->userdata;

	u->match_property_name = pa_modargs_get_value(modargs, ARG_MATCH_PROP_KEY, NULL);
	if (!u->match_property_name)
	{
		pa_log_error("Argument[%s] value is required", ARG_MATCH_PROP_KEY);
		return INIT_FAIL;
	}

	pa_hook_connect(&m->core->hooks[PA_CORE_HOOK_SINK_INPUT_PUT],
			PA_HOOK_EARLY, (pa_hook_cb_t) put_input_to_sink, u);

	pa_hook_connect(&m->core->hooks[PA_CORE_HOOK_SINK_PUT],
			PA_HOOK_EARLY, (pa_hook_cb_t) move_input_to_sink, u);


	pa_log_notice("Initialized module %s : arguments[%s]", m->name, m->argument);
  return INIT_SUCCESS;
}

/* pulse segfaults when trying to unload a module and pa__done is not implemented */
void
pa__done(pa_module* m)
{
	pa_log_notice("Destroying module %s", m->name);
	pa_xfree(m->userdata);
}
