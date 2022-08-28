static AVExpr *make_eval_expr(int type, int value, AVExpr *p0, AVExpr *p1)
{
    AVExpr *e = av_mallocz(sizeof(AVExpr));
    if (!e)
        return NULL;
    e->type     =type   ;
    e->value    =value  ;
    e->param[0] =p0     ;
    e->param[1] =p1     ;
    return e;
}
