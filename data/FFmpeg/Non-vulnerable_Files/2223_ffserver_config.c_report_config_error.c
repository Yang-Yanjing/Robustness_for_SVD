static void report_config_error(const char *filename, int line_num,
                                int log_level, int *errors,
                                const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    vreport_config_error(filename, line_num, log_level, errors, fmt, vl);
    va_end(vl);
}
