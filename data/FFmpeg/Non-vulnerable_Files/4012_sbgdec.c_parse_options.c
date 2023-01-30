static int parse_options(struct sbg_parser *p)
{
    struct sbg_string ostr, oarg;
    char mode = 0;
    int r;
    char *tptr;
    double v;
    if (p->cursor == p->end || *p->cursor != '-')
        return 0;
    while (lex_char(p, '-') && lex_wsword(p, &ostr)) {
        for (; ostr.s < ostr.e; ostr.s++) {
            char opt = *ostr.s;
            switch (opt) {
                case 'S':
                    p->scs.opt_start_at_first = 1;
                    break;
                case 'E':
                    p->scs.opt_end_at_last = 1;
                    break;
                case 'i':
                    mode = 'i';
                    break;
                case 'p':
                    mode = 'p';
                    break;
                case 'F':
                    FORWARD_ERROR(parse_optarg(p, opt, &oarg));
                    v = strtod(oarg.s, &tptr);
                    if (oarg.e != tptr) {
                        snprintf(p->err_msg, sizeof(p->err_msg),
                                 "syntax error for option -F");
                        return AVERROR_INVALIDDATA;
                    }
                    p->scs.opt_fade_time = v * AV_TIME_BASE / 1000;
                    break;
                case 'L':
                    FORWARD_ERROR(parse_optarg(p, opt, &oarg));
                    r = str_to_time(oarg.s, &p->scs.opt_duration);
                    if (oarg.e != oarg.s + r) {
                        snprintf(p->err_msg, sizeof(p->err_msg),
                                 "syntax error for option -L");
                        return AVERROR_INVALIDDATA;
                    }
                    break;
                case 'T':
                    FORWARD_ERROR(parse_optarg(p, opt, &oarg));
                    r = str_to_time(oarg.s, &p->scs.start_ts);
                    if (oarg.e != oarg.s + r) {
                        snprintf(p->err_msg, sizeof(p->err_msg),
                                 "syntax error for option -T");
                        return AVERROR_INVALIDDATA;
                    }
                    break;
                case 'm':
                    FORWARD_ERROR(parse_optarg(p, opt, &oarg));
                    tptr = av_malloc(oarg.e - oarg.s + 1);
                    if (!tptr)
                        return AVERROR(ENOMEM);
                    memcpy(tptr, oarg.s, oarg.e - oarg.s);
                    tptr[oarg.e - oarg.s] = 0;
                    av_free(p->scs.opt_mix);
                    p->scs.opt_mix = tptr;
                    break;
                case 'q':
                    FORWARD_ERROR(parse_optarg(p, opt, &oarg));
                    v = strtod(oarg.s, &tptr);
                    if (oarg.e != tptr) {
                        snprintf(p->err_msg, sizeof(p->err_msg),
                                 "syntax error for option -q");
                        return AVERROR_INVALIDDATA;
                    }
                    if (v != 1) {
                        snprintf(p->err_msg, sizeof(p->err_msg),
                                 "speed factor other than 1 not supported");
                        return AVERROR_PATCHWELCOME;
                    }
                    break;
                case 'r':
                    FORWARD_ERROR(parse_optarg(p, opt, &oarg));
                    r = strtol(oarg.s, &tptr, 10);
                    if (oarg.e != tptr) {
                        snprintf(p->err_msg, sizeof(p->err_msg),
                                 "syntax error for option -r");
                        return AVERROR_INVALIDDATA;
                    }
                    if (r < 40) {
                        snprintf(p->err_msg, sizeof(p->err_msg),
                                 "invalid sample rate");
                        return AVERROR_PATCHWELCOME;
                    }
                    p->scs.sample_rate = r;
                    break;
                default:
                    snprintf(p->err_msg, sizeof(p->err_msg),
                             "unknown option: '%c'", *ostr.s);
                    return AVERROR_INVALIDDATA;
            }
        }
    }
    switch (mode) {
        case 'i':
            return parse_immediate(p);
        case 'p':
            return parse_preprogrammed(p);
        case 0:
            if (!lex_line_end(p))
                return AVERROR_INVALIDDATA;
            return 1;
    }
    return AVERROR_BUG;
}
