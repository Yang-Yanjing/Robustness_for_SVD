static int process_command(AVFilterContext *ctx, const char *cmd, const char *args,
                           char *res, int res_len, int flags)
{
    LADSPA_Data value;
    unsigned long port;
    if (sscanf(cmd, "c%ld", &port) + sscanf(args, "%f", &value) != 2)
        return AVERROR(EINVAL);
    return set_control(ctx, port, value);
}
