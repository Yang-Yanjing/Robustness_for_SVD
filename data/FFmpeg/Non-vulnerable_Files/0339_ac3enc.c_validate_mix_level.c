static void validate_mix_level(void *log_ctx, const char *opt_name,
                               float *opt_param, const float *list,
                               int list_size, int default_value, int min_value,
                               int *ctx_param)
{
    int mixlev = validate_float_option(*opt_param, list, list_size);
    if (mixlev < min_value) {
        mixlev = default_value;
        if (*opt_param >= 0.0) {
            av_log(log_ctx, AV_LOG_WARNING, "requested %s is not valid. using "
                   "default value: %0.3f\n", opt_name, list[mixlev]);
        }
    }
    *opt_param = list[mixlev];
    *ctx_param = mixlev;
}
}
static void validate_mix_level(void *log_ctx, const char *opt_name,
                               float *opt_param, const float *list,
                               int list_size, int default_value, int min_value,
                               int *ctx_param)
{
    int mixlev = validate_float_option(*opt_param, list, list_size);
    if (mixlev < min_value) {
        mixlev = default_value;
        if (*opt_param >= 0.0) {
            av_log(log_ctx, AV_LOG_WARNING, "requested %s is not valid. using "
                   "default value: %0.3f\n", opt_name, list[mixlev]);
        }
    }
