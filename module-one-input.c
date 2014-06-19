#include "module.h"

PA_MODULE_AUTHOR("Ruben Jenster");
PA_MODULE_DESCRIPTION(
	"Removes/Mutes existing inputs of sink when a new input is added."
	"This ensures that only one input is active and multiple inputs do not overlay."
	);
PA_MODULE_LOAD_ONCE(1);
PA_MODULE_VERSION("0.0.1");

static void
disable_existing_inputs(pa_core* core, pa_sink_input* new_input)
{
	pa_sink* sink = new_input->sink;

	pa_log_notice("Input #%u of sink %s gains priority.", new_input->index, sink->name);
	unsigned num_inputs = pa_idxset_size(sink->inputs);

	if (num_inputs > 1)
	{
		pa_log_notice("Muting other sinks (count %u).", num_inputs - 1);

		void* state = NULL;
		pa_sink_input* input = NULL;
		while ((input = (pa_sink_input*)pa_idxset_iterate(sink->inputs, &state, NULL)) != NULL)
		{
			if (input != new_input)
			{
				const char* media_role = pa_proplist_gets(input->proplist, PA_PROP_MEDIA_ROLE);

				/* inputs created by module-combine-sink must be ignored */
				if (media_role && strcmp(media_role, "filter") == 0)
				{
					pa_log_notice("Ignore input #%ld with media.role = 'filter'", input->index);
					continue;
				}

				// FIXME getting "Audio device got stuck!" with mplayer on new connections when killing the input
//				input->kill(input);
				// Mute instead of killing the client
				pa_log_notice("Muting input #%ld of sink %s", input->index, sink->name);
				pa_sink_input_set_mute(input, true, false);
//				input->client->kill(input->client);
//				pa_log_notice("Killing input #%ld of sink %s", input->index, sink->name);
			}
		}
	}
	else
	{
		pa_log_notice("No other existing inputs");
	}
}

static pa_hook_result_t
sink_input_added_cb(pa_core* core, pa_sink_input* i, void* u)
{
	pa_core_assert_ref(core);
	pa_sink_input_assert_ref(i);

	if (!i->sink)
		pa_log_error("SINK INPUT MOVE: No sink set for input.");
	else
		disable_existing_inputs(core, i);

	return PA_HOOK_OK;
}

int
pa__init(pa_module* m)
{
	pa_hook_connect(&m->core->hooks[PA_CORE_HOOK_SINK_INPUT_MOVE_FINISH],
			PA_HOOK_EARLY, (pa_hook_cb_t)sink_input_added_cb, NULL);

	pa_hook_connect(&m->core->hooks[PA_CORE_HOOK_SINK_INPUT_PUT],
			PA_HOOK_EARLY, (pa_hook_cb_t)sink_input_added_cb, NULL);

	pa_log_notice("Initialized module %s : arguments[%s]", m->name, m->argument);
	return INIT_SUCCESS;
}

void
pa__done(pa_module* m)
{
	pa_log_notice("Destroying module %s", m->name);
}
