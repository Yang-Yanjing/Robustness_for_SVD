static int ffserver_parse_config_redirect(FFServerConfig *config,
                                          const char *cmd, const char **p,
                                          FFServerStream **predirect)
{
    FFServerStream *redirect;
    av_assert0(predirect);
    redirect = *predirect;
    if (!av_strcasecmp(cmd, "<Redirect")) {
        char *q;
        redirect = av_mallocz(sizeof(FFServerStream));
        if (!redirect)
            return AVERROR(ENOMEM);
        ffserver_get_arg(redirect->filename, sizeof(redirect->filename), p);
        q = strrchr(redirect->filename, '>');
        if (*q)
            *q = '\0';
        redirect->stream_type = STREAM_TYPE_REDIRECT;
        *predirect = redirect;
        return 0;
    }
    av_assert0(redirect);
    if (!av_strcasecmp(cmd, "URL")) {
        ffserver_get_arg(redirect->feed_filename,
                sizeof(redirect->feed_filename), p);
    } else if (!av_strcasecmp(cmd, "</Redirect>")) {
        if (!redirect->feed_filename[0])
            ERROR("No URL found for <Redirect>\n");
        *predirect = NULL;
    } else {
        ERROR("Invalid entry '%s' inside <Redirect></Redirect>\n", cmd);
    }
    return 0;
}
