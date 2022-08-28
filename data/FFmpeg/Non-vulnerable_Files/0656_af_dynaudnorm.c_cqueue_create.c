static cqueue *cqueue_create(int size)
{
    cqueue *q;
    q = av_malloc(sizeof(cqueue));
    if (!q)
        return NULL;
    q->size = size;
    q->nb_elements = 0;
    q->first = 0;
    q->elements = av_malloc(sizeof(double) * size);
    if (!q->elements) {
        av_free(q);
        return NULL;
    }
    return q;
}
