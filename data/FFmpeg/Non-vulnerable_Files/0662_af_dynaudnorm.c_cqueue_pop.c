static int cqueue_pop(cqueue *q)
{
    av_assert2(!cqueue_empty(q));
    q->first = (q->first + 1) % q->size;
    q->nb_elements--;
    return 0;
}
