static int cqueue_dequeue(cqueue *q, double *element)
{
    av_assert2(!cqueue_empty(q));
    *element = q->elements[q->first];
    q->first = (q->first + 1) % q->size;
    q->nb_elements--;
    return 0;
}
