static int ffserver_parse_config_global(FFServerConfig *config, const char *cmd,
                                        const char **p)
{
    int val;
    char arg[1024];
    if (!av_strcasecmp(cmd, "Port") || !av_strcasecmp(cmd, "HTTPPort")) {
        if (!av_strcasecmp(cmd, "Port"))
            WARNING("Port option is deprecated. Use HTTPPort instead.\n");
        ffserver_get_arg(arg, sizeof(arg), p);
        ffserver_set_int_param(&val, arg, 0, 1, 65535, config,
                "Invalid port: %s\n", arg);
        if (val < 1024)
            WARNING("Trying to use IETF assigned system port: '%d'\n", val);
        config->http_addr.sin_port = htons(val);
    } else if (!av_strcasecmp(cmd, "HTTPBindAddress") ||
               !av_strcasecmp(cmd, "BindAddress")) {
        if (!av_strcasecmp(cmd, "BindAddress"))
            WARNING("BindAddress option is deprecated. Use HTTPBindAddress "
                    "instead.\n");
        ffserver_get_arg(arg, sizeof(arg), p);
        if (resolve_host(&config->http_addr.sin_addr, arg))
            ERROR("Invalid host/IP address: '%s'\n", arg);
    } else if (!av_strcasecmp(cmd, "NoDaemon")) {
        WARNING("NoDaemon option has no effect. You should remove it.\n");
    } else if (!av_strcasecmp(cmd, "RTSPPort")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        ffserver_set_int_param(&val, arg, 0, 1, 65535, config,
                "Invalid port: %s\n", arg);
        config->rtsp_addr.sin_port = htons(val);
    } else if (!av_strcasecmp(cmd, "RTSPBindAddress")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        if (resolve_host(&config->rtsp_addr.sin_addr, arg))
            ERROR("Invalid host/IP address: %s\n", arg);
    } else if (!av_strcasecmp(cmd, "MaxHTTPConnections")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        ffserver_set_int_param(&val, arg, 0, 1, 65535, config,
                "Invalid MaxHTTPConnections: %s\n", arg);
        config->nb_max_http_connections = val;
        if (config->nb_max_connections > config->nb_max_http_connections) {
            ERROR("Inconsistent configuration: MaxClients(%d) > "
                  "MaxHTTPConnections(%d)\n", config->nb_max_connections,
                  config->nb_max_http_connections);
        }
    } else if (!av_strcasecmp(cmd, "MaxClients")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        ffserver_set_int_param(&val, arg, 0, 1, 65535, config,
                "Invalid MaxClients: '%s'\n", arg);
        config->nb_max_connections = val;
        if (config->nb_max_connections > config->nb_max_http_connections) {
            ERROR("Inconsistent configuration: MaxClients(%d) > "
                  "MaxHTTPConnections(%d)\n", config->nb_max_connections,
                  config->nb_max_http_connections);
        }
    } else if (!av_strcasecmp(cmd, "MaxBandwidth")) {
        int64_t llval;
        char *tailp;
        ffserver_get_arg(arg, sizeof(arg), p);
        errno = 0;
        llval = strtoll(arg, &tailp, 10);
        if (llval < 10 || llval > 10000000 || tailp[0] || errno)
            ERROR("Invalid MaxBandwidth: '%s'\n", arg);
        else
            config->max_bandwidth = llval;
    } else if (!av_strcasecmp(cmd, "CustomLog")) {
        if (!config->debug) {
            ffserver_get_arg(config->logfilename, sizeof(config->logfilename),
                             p);
        }
    } else if (!av_strcasecmp(cmd, "LoadModule")) {
        ERROR("Loadable modules are no longer supported\n");
    } else if (!av_strcasecmp(cmd, "NoDefaults")) {
        config->use_defaults = 0;
    } else if (!av_strcasecmp(cmd, "UseDefaults")) {
        config->use_defaults = 1;
    } else
        ERROR("Incorrect keyword: '%s'\n", cmd);
    return 0;
}
