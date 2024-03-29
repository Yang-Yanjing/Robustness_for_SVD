int avfilter_init_str(AVFilterContext *filter, const char *args)
{
    AVDictionary *options = NULL;
    AVDictionaryEntry *e;
    int ret = 0;
    if (args && *args) {
        if (!filter->filter->priv_class) {
            av_log(filter, AV_LOG_ERROR, "This filter does not take any "
                   "options, but options were provided: %s.\n", args);
            return AVERROR(EINVAL);
        }
#if FF_API_OLD_FILTER_OPTS || FF_API_OLD_FILTER_OPTS_ERROR
            if (   !strcmp(filter->filter->name, "format")     ||
                   !strcmp(filter->filter->name, "noformat")   ||
                   !strcmp(filter->filter->name, "frei0r")     ||
                   !strcmp(filter->filter->name, "frei0r_src") ||
                   !strcmp(filter->filter->name, "ocv")        ||
                   !strcmp(filter->filter->name, "pan")        ||
                   !strcmp(filter->filter->name, "pp")         ||
                   !strcmp(filter->filter->name, "aevalsrc")) {
            

            char *copy = av_strdup(args);
            char *p    = copy;
            int nb_leading = 0; 
            int deprecated = 0;
            if (!copy) {
                ret = AVERROR(ENOMEM);
                goto fail;
            }
            if (!strcmp(filter->filter->name, "frei0r") ||
                !strcmp(filter->filter->name, "ocv"))
                nb_leading = 1;
            else if (!strcmp(filter->filter->name, "frei0r_src"))
                nb_leading = 3;
            while (nb_leading--) {
                p = strchr(p, ':');
                if (!p) {
                    p = copy + strlen(copy);
                    break;
                }
                p++;
            }
            deprecated = strchr(p, ':') != NULL;
            if (!strcmp(filter->filter->name, "aevalsrc")) {
                deprecated = 0;
                while ((p = strchr(p, ':')) && p[1] != ':') {
                    const char *epos = strchr(p + 1, '=');
                    const char *spos = strchr(p + 1, ':');
                    const int next_token_is_opt = epos && (!spos || epos < spos);
                    if (next_token_is_opt) {
                        p++;
                        break;
                    }
                    
                    deprecated = 1;
                    *p++ = '|';
                }
                if (p && *p == ':') { 
                    deprecated = 1;
                    memmove(p, p + 1, strlen(p));
                }
            } else
            while ((p = strchr(p, ':')))
                *p++ = '|';
#if FF_API_OLD_FILTER_OPTS
            if (deprecated)
                av_log(filter, AV_LOG_WARNING, "This syntax is deprecated. Use "
                       "'|' to separate the list items.\n");
            av_log(filter, AV_LOG_DEBUG, "compat: called with args=[%s]\n", copy);
            ret = process_options(filter, &options, copy);
#else
            if (deprecated) {
                av_log(filter, AV_LOG_ERROR, "This syntax is deprecated. Use "
                       "'|' to separate the list items ('%s' instead of '%s')\n",
                       copy, args);
                ret = AVERROR(EINVAL);
            } else {
                ret = process_options(filter, &options, copy);
            }
#endif
            av_freep(&copy);
            if (ret < 0)
                goto fail;
        } else
#endif
        {
            ret = process_options(filter, &options, args);
            if (ret < 0)
                goto fail;
        }
    }
    ret = avfilter_init_dict(filter, &options);
    if (ret < 0)
        goto fail;
    if ((e = av_dict_get(options, "", NULL, AV_DICT_IGNORE_SUFFIX))) {
        av_log(filter, AV_LOG_ERROR, "No such option: %s.\n", e->key);
        ret = AVERROR_OPTION_NOT_FOUND;
        goto fail;
    }
fail:
    av_dict_free(&options);
    return ret;
}
