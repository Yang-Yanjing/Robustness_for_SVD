static double cqueue_peek(cqueue *q, int index)
{
    av_assert2(index < q->nb_elements);
    return q->elements[(q->first + index) % q->size];
}
