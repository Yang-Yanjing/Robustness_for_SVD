static av_cold int frei0r_init(AVFilterContext *ctx,
                               const char *dl_name, int type)
{
    Frei0rContext *s = ctx->priv;
    f0r_init_f            f0r_init;
    f0r_get_plugin_info_f f0r_get_plugin_info;
    f0r_plugin_info_t *pi;
    char *path;
    int ret = 0;
    int i;
    static const char* const frei0r_pathlist[] = {
        "/usr/local/lib/frei0r-1/",
        "/usr/lib/frei0r-1/",
        "/usr/local/lib64/frei0r-1/",
        "/usr/lib64/frei0r-1/"
    };
    if (!dl_name) {
        av_log(ctx, AV_LOG_ERROR, "No filter name provided.\n");
        return AVERROR(EINVAL);
    }
    
    if ((path = av_strdup(getenv("FREI0R_PATH")))) {
#ifdef _WIN32
        const char *separator = ";";
#else
        const char *separator = ":";
#endif
        char *p, *ptr = NULL;
        for (p = path; p = av_strtok(p, separator, &ptr); p = NULL) {
            
            char *p1 = av_asprintf("%s/", p);
            if (!p1) {
                ret = AVERROR(ENOMEM);
                goto check_path_end;
            }
            ret = load_path(ctx, &s->dl_handle, p1, dl_name);
            av_free(p1);
            if (ret < 0)
                goto check_path_end;
            if (s->dl_handle)
                break;
        }
    check_path_end:
        av_free(path);
        if (ret < 0)
            return ret;
    }
    if (!s->dl_handle && (path = getenv("HOME"))) {
        char *prefix = av_asprintf("%s/.frei0r-1/lib/", path);
        if (!prefix)
            return AVERROR(ENOMEM);
        ret = load_path(ctx, &s->dl_handle, prefix, dl_name);
        av_free(prefix);
        if (ret < 0)
            return ret;
    }
    for (i = 0; !s->dl_handle && i < FF_ARRAY_ELEMS(frei0r_pathlist); i++) {
        ret = load_path(ctx, &s->dl_handle, frei0r_pathlist[i], dl_name);
        if (ret < 0)
            return ret;
    }
    if (!s->dl_handle) {
        av_log(ctx, AV_LOG_ERROR, "Could not find module '%s'.\n", dl_name);
        return AVERROR(EINVAL);
    }
    if (!(f0r_init                = load_sym(ctx, "f0r_init"           )) ||
        !(f0r_get_plugin_info     = load_sym(ctx, "f0r_get_plugin_info")) ||
        !(s->get_param_info  = load_sym(ctx, "f0r_get_param_info" )) ||
        !(s->get_param_value = load_sym(ctx, "f0r_get_param_value")) ||
        !(s->set_param_value = load_sym(ctx, "f0r_set_param_value")) ||
        !(s->update          = load_sym(ctx, "f0r_update"         )) ||
        !(s->construct       = load_sym(ctx, "f0r_construct"      )) ||
        !(s->destruct        = load_sym(ctx, "f0r_destruct"       )) ||
        !(s->deinit          = load_sym(ctx, "f0r_deinit"         )))
        return AVERROR(EINVAL);
    if (f0r_init() < 0) {
        av_log(ctx, AV_LOG_ERROR, "Could not init the frei0r module.\n");
        return AVERROR(EINVAL);
    }
    f0r_get_plugin_info(&s->plugin_info);
    pi = &s->plugin_info;
    if (pi->plugin_type != type) {
        av_log(ctx, AV_LOG_ERROR,
               "Invalid type '%s' for this plugin\n",
               pi->plugin_type == F0R_PLUGIN_TYPE_FILTER ? "filter" :
               pi->plugin_type == F0R_PLUGIN_TYPE_SOURCE ? "source" :
               pi->plugin_type == F0R_PLUGIN_TYPE_MIXER2 ? "mixer2" :
               pi->plugin_type == F0R_PLUGIN_TYPE_MIXER3 ? "mixer3" : "unknown");
        return AVERROR(EINVAL);
    }
    av_log(ctx, AV_LOG_VERBOSE,
           "name:%s author:'%s' explanation:'%s' color_model:%s "
           "frei0r_version:%d version:%d.%d num_params:%d\n",
           pi->name, pi->author, pi->explanation,
           pi->color_model == F0R_COLOR_MODEL_BGRA8888 ? "bgra8888" :
           pi->color_model == F0R_COLOR_MODEL_RGBA8888 ? "rgba8888" :
           pi->color_model == F0R_COLOR_MODEL_PACKED32 ? "packed32" : "unknown",
           pi->frei0r_version, pi->major_version, pi->minor_version, pi->num_params);
    return 0;
}
