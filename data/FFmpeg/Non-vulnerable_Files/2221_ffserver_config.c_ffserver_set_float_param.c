static int ffserver_set_float_param(float *dest, const char *value,
                                    float factor, float min, float max,
                                    FFServerConfig *config,
                                    const char *error_msg, ...)
{
    double tmp;
    char *tailp;
    if (!value || !value[0])
        goto error;
    errno = 0;
    tmp = strtod(value, &tailp);
    if (tmp < min || tmp > max)
        goto error;
    if (factor)
        tmp *= factor;
    if (tailp[0] || errno)
        goto error;
    if (dest)
        *dest = tmp;
    return 0;
  error:
    if (config) {
        va_list vl;
        va_start(vl, error_msg);
        vreport_config_error(config->filename, config->line_num, AV_LOG_ERROR,
                &config->errors, error_msg, vl);
        va_end(vl);
    }
    return AVERROR(EINVAL);
}
