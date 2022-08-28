static double gaussian_filter(DynamicAudioNormalizerContext *s, cqueue *q)
{
    double result = 0.0;
    int i;
    for (i = 0; i < cqueue_size(q); i++) {
        result += cqueue_peek(q, i) * s->weights[i];
    }
    return result;
}
