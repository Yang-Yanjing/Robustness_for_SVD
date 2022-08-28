static int open_files(OptionGroupList *l, const char *inout,
                      int (*open_file)(OptionsContext*, const char*))
{
    int i, ret;
    for (i = 0; i < l->nb_groups; i++) {
        OptionGroup *g = &l->groups[i];
        OptionsContext o;
        init_options(&o);
        o.g = g;
        ret = parse_optgroup(&o, g);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Error parsing options for %s file "
                   "%s.\n", inout, g->arg);
            return ret;
        }
        av_log(NULL, AV_LOG_DEBUG, "Opening an %s file: %s.\n", inout, g->arg);
        ret = open_file(&o, g->arg);
        uninit_options(&o);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Error opening %s file %s.\n",
                   inout, g->arg);
            return ret;
        }
        av_log(NULL, AV_LOG_DEBUG, "Successfully opened the file.\n");
    }
    return 0;
}
