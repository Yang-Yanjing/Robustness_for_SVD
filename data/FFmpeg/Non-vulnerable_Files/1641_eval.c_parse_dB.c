static int parse_dB(AVExpr **e, Parser *p, int *sign)
{
    

    if (*p->s == '-') {
        char *next;
        double av_unused ignored = strtod(p->s, &next);
        if (next != p->s && next[0] == 'd' && next[1] == 'B') {
            *sign = 0;
            return parse_primary(e, p);
        }
    }
    return parse_pow(e, p, sign);
}
