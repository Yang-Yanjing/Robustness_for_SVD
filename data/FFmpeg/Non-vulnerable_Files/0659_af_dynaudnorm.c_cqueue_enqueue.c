static int cqueue_enqueue(cqueue *q, double element)
{
    int i;
    av_assert2(q->nb_elements != q->size);
    i = (q->first + q->nb_elements) % q->size;
    q->elements[i] = element;
    q->nb_elements++;
    return 0;
}
