static int parse_pow(AVExpr **e, Parser *p, int *sign)
{
    *sign= (*p->s == '+') - (*p->s == '-');
    p->s += *sign&1;
    return parse_primary(e, p);
}
