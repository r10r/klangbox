#include "module.h"
#include "util.h"

PA_MODULE_AUTHOR("Ruben Jenster");
PA_MODULE_DESCRIPTION("Ensure that a sink is only included a single combine sink");
PA_MODULE_LOAD_ONCE(1);
PA_MODULE_VERSION("0.0.1");

#define is_combine_sink(s) \
	(strcmp((s)->driver, "module-combine-sink.c") == 0)

#define SLAVES_SEPARATOR ","
#define PA_PROP_COMBINE_SLAVES "combine.slaves"

static void
recreate_with_slaves(pa_sink* sink, const char** slave_names)
{
	char* slave_names_str = str_arr_join(slave_names, SLAVES_SEPARATOR);

	pa_log_notice("SINK[%s] - recreate with slaves[%s]", sink->name, slave_names_str);

	// TODO copy sink properties (e.g application.process.user )
	const char* user = pa_proplist_gets(sink->proplist, PA_PROP_APPLICATION_PROCESS_USER);

	const char* argument = pa_sprintf_malloc(
		"slaves=%s "
		"sink_properties=%s=%s",
		slave_names_str,
		PA_PROP_APPLICATION_PROCESS_USER, user);

	pa_log_notice("LOAD MODULE [combine-sink] argument[%s]", argument);

	pa_module_load(sink->core, "module-combine-sink", argument);
	pa_module_unload(sink->core, sink->module, false);

	free(slave_names_str);
}

static void
try_remove_slaves_from_sink(pa_sink* sink, const char** slave_names_to_remove)
{
	char* slave_names_str = strdup(pa_proplist_gets(sink->proplist, PA_PROP_COMBINE_SLAVES));

	pa_log_notice("SINK[%s] slaves[%s] - try remove slaves", sink->name, slave_names_str);

	const char** slave_names = NULL;
	unsigned num_slave_names = str_split(slave_names_str, SLAVES_SEPARATOR, &slave_names);


	if (!slave_names)
	{
		pa_log_error("SINK[%s] - has no slaves", sink->name);
	}
	else
	{
		const char** slaves_remaining;
		unsigned num_slaves_remaining = str_arr_diff(slave_names, num_slave_names, slave_names_to_remove, &slaves_remaining);
		pa_log_notice("SINK[%s] - %u slaves remaining", sink->name, num_slaves_remaining);

		if (num_slaves_remaining)
		{
			recreate_with_slaves(sink, (const char**)slaves_remaining);
		}
		else
		{
			pa_log_notice("SINK[%s] - destroy (no slaves remaining)", sink->name);
			// TODO kill input and unload module
		}
		free(slaves_remaining);
	}
	free(slave_names_str);
}

static pa_hook_result_t
new_sink_cb(pa_core* core, pa_sink* new_sink, void* u)
{
	(void)u;

	if (is_combine_sink(new_sink))
	{
		const char* slave_names_prop = pa_proplist_gets(new_sink->proplist, PA_PROP_COMBINE_SLAVES);

		if (!slave_names_prop)
		{
			pa_log_error("SINK[%s] - has no slaves", new_sink->name);
			return PA_HOOK_OK;
		}
		else
		{
			char* slave_names_str = strdup(slave_names_prop);
			pa_log_notice("SINK[%s] - new slaves[%s]", new_sink->name, slave_names_str);

			const char** slave_names = NULL;
			str_split(slave_names_str, SLAVES_SEPARATOR, &slave_names);

			if (slave_names && pa_idxset_size(core->sinks) > 1)
			{
				pa_log_notice("SINK[%s] - try removing slaves from existing combine sinks", new_sink->name);
				void* state = NULL;
				pa_sink* sink = NULL;
				while ((sink = (pa_sink*)pa_idxset_iterate(core->sinks, &state, NULL)) != NULL)
				{
					if (sink != new_sink && is_combine_sink(sink))
						try_remove_slaves_from_sink(sink, slave_names);
				}
			}
			else
			{
				pa_log_notice("SINK[%s] - no other sinks loaded", new_sink->name);
			}

			free(slave_names);
			free(slave_names_str);
		}
	}

	return PA_HOOK_OK;
}

int
pa__init(pa_module* m)
{
	pa_hook_connect(&m->core->hooks[PA_CORE_HOOK_SINK_PUT],
			PA_HOOK_EARLY, (pa_hook_cb_t)new_sink_cb, NULL);

	pa_log_notice("Initialized module %s : arguments[%s]", m->name, m->argument);
	return INIT_SUCCESS;
}

void
pa__done(pa_module* m)
{
	pa_log_notice("Destroying module %s", m->name);
}
