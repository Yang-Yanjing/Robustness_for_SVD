static void vreport_config_error(const char *filename, int line_num,
                                 int log_level, int *errors, const char *fmt,
                                 va_list vl)
{
    av_log(NULL, log_level, "%s:%d: ", filename, line_num);
    av_vlog(NULL, log_level, fmt, vl);
    if (errors)
        (*errors)++;
}
