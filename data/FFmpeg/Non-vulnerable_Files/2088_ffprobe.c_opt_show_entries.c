static int opt_show_entries(void *optctx, const char *opt, const char *arg)
{
    const char *p = arg;
    int ret = 0;
    while (*p) {
        AVDictionary *entries = NULL;
        char *section_name = av_get_token(&p, "=:");
        int show_all_entries = 0;
        if (!section_name) {
            av_log(NULL, AV_LOG_ERROR,
                   "Missing section name for option '%s'\n", opt);
            return AVERROR(EINVAL);
        }
        if (*p == '=') {
            p++;
            while (*p && *p != ':') {
                char *entry = av_get_token(&p, ",:");
                if (!entry)
                    break;
                av_log(NULL, AV_LOG_VERBOSE,
                       "Adding '%s' to the entries to show in section '%s'\n",
                       entry, section_name);
                av_dict_set(&entries, entry, "", AV_DICT_DONT_STRDUP_KEY);
                if (*p == ',')
                    p++;
            }
        } else {
            show_all_entries = 1;
        }
        ret = match_section(section_name, show_all_entries, entries);
        if (ret == 0) {
            av_log(NULL, AV_LOG_ERROR, "No match for section '%s'\n", section_name);
            ret = AVERROR(EINVAL);
        }
        av_dict_free(&entries);
        av_free(section_name);
        if (ret <= 0)
            break;
        if (*p)
            p++;
    }
    return ret;
}
