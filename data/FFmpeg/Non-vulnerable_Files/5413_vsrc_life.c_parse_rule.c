AVFILTER_DEFINE_CLASS(life);
static int parse_rule(uint16_t *born_rule, uint16_t *stay_rule,
                      const char *rule_str, void *log_ctx)
{
    char *tail;
    const char *p = rule_str;
    *born_rule = 0;
    *stay_rule = 0;
    if (strchr("bBsS", *p)) {
        

        do {
            uint16_t *rule = (*p == 'b' || *p == 'B') ? born_rule : stay_rule;
            p++;
            while (*p >= '0' && *p <= '8') {
                *rule += 1<<(*p - '0');
                p++;
            }
            if (*p != '/')
                break;
            p++;
        } while (strchr("bBsS", *p));
        if (*p)
            goto error;
    } else {
        

        long int rule = strtol(rule_str, &tail, 10);
        if (*tail)
            goto error;
        *born_rule  = ((1<<9)-1) & rule;
        *stay_rule = rule >> 9;
    }
    return 0;
error:
    av_log(log_ctx, AV_LOG_ERROR, "Invalid rule code '%s' provided\n", rule_str);
    return AVERROR(EINVAL);
}
