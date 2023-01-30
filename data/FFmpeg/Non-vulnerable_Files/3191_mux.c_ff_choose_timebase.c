AVRational ff_choose_timebase(AVFormatContext *s, AVStream *st, int min_precision)
{
    AVRational q;
    int j;
    q = st->time_base;
    for (j=2; j<14; j+= 1+(j>2))
        while (q.den / q.num < min_precision && q.num % j == 0)
            q.num /= j;
    while (q.den / q.num < min_precision && q.den < (1<<24))
        q.den <<= 1;
    return q;
}
