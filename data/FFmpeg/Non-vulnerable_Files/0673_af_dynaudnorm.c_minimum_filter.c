static double minimum_filter(cqueue *q)
{
    double min = DBL_MAX;
    int i;
    for (i = 0; i < cqueue_size(q); i++) {
        min = FFMIN(min, cqueue_peek(q, i));
    }
    return min;
}
