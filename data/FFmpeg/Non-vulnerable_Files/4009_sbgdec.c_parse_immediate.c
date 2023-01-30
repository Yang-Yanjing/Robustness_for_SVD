static int parse_immediate(struct sbg_parser *p)
{
    snprintf(p->err_msg, sizeof(p->err_msg),
             "immediate sequences not yet implemented");
    return AVERROR_PATCHWELCOME;
}
