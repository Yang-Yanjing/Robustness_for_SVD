static int parse_term(AVExpr **e, Parser *p)
{
    int ret;
    AVExpr *e0, *e1, *e2;
    if ((ret = parse_factor(&e0, p)) < 0)
        return ret;
    while (p->s[0]=='*' || p->s[0]=='/') {
        int c= *p->s++;
        e1 = e0;
        if ((ret = parse_factor(&e2, p)) < 0) {
            av_expr_free(e1);
            return ret;
        }
        e0 = make_eval_expr(c == '*' ? e_mul : e_div, 1, e1, e2);
        if (!e0) {
            av_expr_free(e1);
            av_expr_free(e2);
            return AVERROR(ENOMEM);
        }
    }
    *e = e0;
    return 0;
}
