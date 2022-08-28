static int cqueue_empty(cqueue *q)
{
    return !q->nb_elements;
}
