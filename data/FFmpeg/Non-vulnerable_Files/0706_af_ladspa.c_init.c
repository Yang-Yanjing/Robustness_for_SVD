static av_cold int init(AVFilterContext *ctx)
{
    LADSPAContext *s = ctx->priv;
    LADSPA_Descriptor_Function descriptor_fn;
    const LADSPA_Descriptor *desc;
    LADSPA_PortDescriptor pd;
    AVFilterPad pad = { NULL };
    char *p, *arg, *saveptr = NULL;
    unsigned long nb_ports;
    int i;
    if (!s->dl_name) {
        av_log(ctx, AV_LOG_ERROR, "No plugin name provided\n");
        return AVERROR(EINVAL);
    }
    if (s->dl_name[0] == '/' || s->dl_name[0] == '.') {
        
        s->dl_handle = dlopen(s->dl_name, RTLD_LOCAL|RTLD_NOW);
    } else {
        
        char *paths = av_strdup(getenv("LADSPA_PATH"));
        const char *separator = ":";
        if (paths) {
            p = paths;
            while ((arg = av_strtok(p, separator, &saveptr)) && !s->dl_handle) {
                s->dl_handle = try_load(arg, s->dl_name);
                p = NULL;
            }
        }
        av_free(paths);
        if (!s->dl_handle && (paths = av_asprintf("%s/.ladspa/lib", getenv("HOME")))) {
            s->dl_handle = try_load(paths, s->dl_name);
            av_free(paths);
        }
        if (!s->dl_handle)
            s->dl_handle = try_load("/usr/local/lib/ladspa", s->dl_name);
        if (!s->dl_handle)
            s->dl_handle = try_load("/usr/lib/ladspa", s->dl_name);
    }
    if (!s->dl_handle) {
        av_log(ctx, AV_LOG_ERROR, "Failed to load '%s'\n", s->dl_name);
        return AVERROR(EINVAL);
    }
    descriptor_fn = dlsym(s->dl_handle, "ladspa_descriptor");
    if (!descriptor_fn) {
        av_log(ctx, AV_LOG_ERROR, "Could not find ladspa_descriptor: %s\n", dlerror());
        return AVERROR(EINVAL);
    }
    
    if (!s->plugin) {
        av_log(ctx, AV_LOG_INFO, "The '%s' library contains the following plugins:\n", s->dl_name);
        av_log(ctx, AV_LOG_INFO, "I = Input Channels\n");
        av_log(ctx, AV_LOG_INFO, "O = Output Channels\n");
        av_log(ctx, AV_LOG_INFO, "I:O %-25s %s\n", "Plugin", "Description");
        av_log(ctx, AV_LOG_INFO, "\n");
        for (i = 0; desc = descriptor_fn(i); i++) {
            unsigned long inputs = 0, outputs = 0;
            count_ports(desc, &inputs, &outputs);
            av_log(ctx, AV_LOG_INFO, "%lu:%lu %-25s %s\n", inputs, outputs, desc->Label,
                   (char *)av_x_if_null(desc->Name, "?"));
            av_log(ctx, AV_LOG_VERBOSE, "Maker: %s\n",
                   (char *)av_x_if_null(desc->Maker, "?"));
            av_log(ctx, AV_LOG_VERBOSE, "Copyright: %s\n",
                   (char *)av_x_if_null(desc->Copyright, "?"));
        }
        return AVERROR_EXIT;
    } else {
        for (i = 0;; i++) {
            desc = descriptor_fn(i);
            if (!desc) {
                av_log(ctx, AV_LOG_ERROR, "Could not find plugin: %s\n", s->plugin);
                return AVERROR(EINVAL);
            }
            if (desc->Label && !strcmp(desc->Label, s->plugin))
                break;
        }
    }
    s->desc  = desc;
    nb_ports = desc->PortCount;
    s->ipmap = av_calloc(nb_ports, sizeof(*s->ipmap));
    s->opmap = av_calloc(nb_ports, sizeof(*s->opmap));
    s->icmap = av_calloc(nb_ports, sizeof(*s->icmap));
    s->ocmap = av_calloc(nb_ports, sizeof(*s->ocmap));
    s->ictlv = av_calloc(nb_ports, sizeof(*s->ictlv));
    s->octlv = av_calloc(nb_ports, sizeof(*s->octlv));
    s->ctl_needs_value = av_calloc(nb_ports, sizeof(*s->ctl_needs_value));
    if (!s->ipmap || !s->opmap || !s->icmap ||
        !s->ocmap || !s->ictlv || !s->octlv || !s->ctl_needs_value)
        return AVERROR(ENOMEM);
    for (i = 0; i < nb_ports; i++) {
        pd = desc->PortDescriptors[i];
        if (LADSPA_IS_PORT_AUDIO(pd)) {
            if (LADSPA_IS_PORT_INPUT(pd)) {
                s->ipmap[s->nb_inputs] = i;
                s->nb_inputs++;
            } else if (LADSPA_IS_PORT_OUTPUT(pd)) {
                s->opmap[s->nb_outputs] = i;
                s->nb_outputs++;
            }
        } else if (LADSPA_IS_PORT_CONTROL(pd)) {
            if (LADSPA_IS_PORT_INPUT(pd)) {
                s->icmap[s->nb_inputcontrols] = i;
                if (LADSPA_IS_HINT_HAS_DEFAULT(desc->PortRangeHints[i].HintDescriptor))
                    set_default_ctl_value(s, s->nb_inputcontrols, s->icmap, s->ictlv);
                else
                    s->ctl_needs_value[s->nb_inputcontrols] = 1;
                s->nb_inputcontrols++;
            } else if (LADSPA_IS_PORT_OUTPUT(pd)) {
                s->ocmap[s->nb_outputcontrols] = i;
                s->nb_outputcontrols++;
            }
        }
    }
    
    if (s->options && !strcmp(s->options, "help")) {
        if (!s->nb_inputcontrols) {
            av_log(ctx, AV_LOG_INFO,
                   "The '%s' plugin does not have any input controls.\n",
                   desc->Label);
        } else {
            av_log(ctx, AV_LOG_INFO,
                   "The '%s' plugin has the following input controls:\n",
                   desc->Label);
            for (i = 0; i < s->nb_inputcontrols; i++)
                print_ctl_info(ctx, AV_LOG_INFO, s, i, s->icmap, s->ictlv, 0);
        }
        return AVERROR_EXIT;
    }
    
    p = s->options;
    while (s->options) {
        LADSPA_Data val;
        int ret;
        if (!(arg = av_strtok(p, "|", &saveptr)))
            break;
        p = NULL;
        if (sscanf(arg, "c%d=%f", &i, &val) != 2) {
            av_log(ctx, AV_LOG_ERROR, "Invalid syntax.\n");
            return AVERROR(EINVAL);
        }
        if ((ret = set_control(ctx, i, val)) < 0)
            return ret;
        s->ctl_needs_value[i] = 0;
    }
    
    for (i = 0; i < s->nb_inputcontrols; i++) {
        if (s->ctl_needs_value[i]) {
            av_log(ctx, AV_LOG_ERROR, "Control c%d must be set.\n", i);
            print_ctl_info(ctx, AV_LOG_ERROR, s, i, s->icmap, s->ictlv, 0);
            return AVERROR(EINVAL);
        }
    }
    pad.type = AVMEDIA_TYPE_AUDIO;
    if (s->nb_inputs) {
        pad.name = av_asprintf("in0:%s%lu", desc->Label, s->nb_inputs);
        if (!pad.name)
            return AVERROR(ENOMEM);
        pad.filter_frame = filter_frame;
        pad.config_props = config_input;
        if (ff_insert_inpad(ctx, ctx->nb_inputs, &pad) < 0) {
            av_freep(&pad.name);
            return AVERROR(ENOMEM);
        }
    }
    av_log(ctx, AV_LOG_DEBUG, "ports: %lu\n", nb_ports);
    av_log(ctx, AV_LOG_DEBUG, "inputs: %lu outputs: %lu\n",
                              s->nb_inputs, s->nb_outputs);
    av_log(ctx, AV_LOG_DEBUG, "input controls: %lu output controls: %lu\n",
                              s->nb_inputcontrols, s->nb_outputcontrols);
    return 0;
}
