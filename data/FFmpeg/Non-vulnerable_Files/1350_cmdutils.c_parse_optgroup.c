int parse_optgroup(void *optctx, OptionGroup *g)
{
    int i, ret;
    av_log(NULL, AV_LOG_DEBUG, "Parsing a group of options: %s %s.\n",
           g->group_def->name, g->arg);
    for (i = 0; i < g->nb_opts; i++) {
        Option *o = &g->opts[i];
        if (g->group_def->flags &&
            !(g->group_def->flags & o->opt->flags)) {
            av_log(NULL, AV_LOG_ERROR, "Option %s (%s) cannot be applied to "
                   "%s %s -- you are trying to apply an input option to an "
                   "output file or vice versa. Move this option before the "
                   "file it belongs to.\n", o->key, o->opt->help,
                   g->group_def->name, g->arg);
            return AVERROR(EINVAL);
        }
        av_log(NULL, AV_LOG_DEBUG, "Applying option %s (%s) with argument %s.\n",
               o->key, o->opt->help, o->val);
        ret = write_option(optctx, o->opt, o->key, o->val);
        if (ret < 0)
            return ret;
    }
    av_log(NULL, AV_LOG_DEBUG, "Successfully parsed a group of options.\n");
    return 0;
}
