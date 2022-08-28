static AVFrame *select_frame(FieldMatchContext *fm, int match)
{
    if      (match == mP || match == mB) return fm->prv;
    else if (match == mN || match == mU) return fm->nxt;
    else                return fm->src;
}
