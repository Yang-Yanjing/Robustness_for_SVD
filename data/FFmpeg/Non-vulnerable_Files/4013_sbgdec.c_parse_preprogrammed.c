static int parse_preprogrammed(struct sbg_parser *p)
{
    snprintf(p->err_msg, sizeof(p->err_msg),
             "preprogrammed sequences not yet implemented");
    return AVERROR_PATCHWELCOME;
}
