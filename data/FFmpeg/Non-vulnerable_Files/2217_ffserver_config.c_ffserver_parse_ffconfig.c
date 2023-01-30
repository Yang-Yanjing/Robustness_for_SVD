int ffserver_parse_ffconfig(const char *filename, FFServerConfig *config)
{
    FILE *f;
    char line[1024];
    char cmd[64];
    const char *p;
    FFServerStream **last_stream, *stream = NULL, *redirect = NULL;
    FFServerStream **last_feed, *feed = NULL;
    int ret = 0;
    av_assert0(config);
    f = fopen(filename, "r");
    if (!f) {
        ret = AVERROR(errno);
        av_log(NULL, AV_LOG_ERROR,
                "Could not open the configuration file '%s'\n", filename);
        return ret;
    }
    config->first_stream = NULL;
    config->first_feed = NULL;
    config->errors = config->warnings = 0;
    last_stream = &config->first_stream;
    last_feed = &config->first_feed;
    config->line_num = 0;
    while (fgets(line, sizeof(line), f) != NULL) {
        config->line_num++;
        p = line;
        while (av_isspace(*p))
            p++;
        if (*p == '\0' || *p == '#')
            continue;
        ffserver_get_arg(cmd, sizeof(cmd), &p);
        if (feed || !av_strcasecmp(cmd, "<Feed")) {
            int opening = !av_strcasecmp(cmd, "<Feed");
            if (opening && (stream || feed || redirect)) {
                ERROR("Already in a tag\n");
            } else {
                ret = ffserver_parse_config_feed(config, cmd, &p, &feed);
                if (ret < 0)
                    break;
                if (opening) {
                    
                    *last_stream = feed;
                    *last_feed = feed;
                    last_stream = &feed->next;
                    last_feed = &feed->next_feed;
                }
            }
        } else if (stream || !av_strcasecmp(cmd, "<Stream")) {
            int opening = !av_strcasecmp(cmd, "<Stream");
            if (opening && (stream || feed || redirect)) {
                ERROR("Already in a tag\n");
            } else {
                ret = ffserver_parse_config_stream(config, cmd, &p, &stream);
                if (ret < 0)
                    break;
                if (opening) {
                    
                    *last_stream = stream;
                    last_stream = &stream->next;
                }
            }
        } else if (redirect || !av_strcasecmp(cmd, "<Redirect")) {
            int opening = !av_strcasecmp(cmd, "<Redirect");
            if (opening && (stream || feed || redirect))
                ERROR("Already in a tag\n");
            else {
                ret = ffserver_parse_config_redirect(config, cmd, &p,
                                                     &redirect);
                if (ret < 0)
                    break;
                if (opening) {
                    
                    *last_stream = redirect;
                    last_stream = &redirect->next;
                }
            }
        } else {
            ffserver_parse_config_global(config, cmd, &p);
        }
    }
    if (stream || feed || redirect)
        ERROR("Missing closing </%s> tag\n",
              stream ? "Stream" : (feed ? "Feed" : "Redirect"));
    fclose(f);
    if (ret < 0)
        return ret;
    if (config->errors)
        return AVERROR(EINVAL);
    else
        return 0;
}
