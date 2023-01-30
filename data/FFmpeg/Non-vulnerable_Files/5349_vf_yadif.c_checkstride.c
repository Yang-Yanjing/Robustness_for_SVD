static int checkstride(YADIFContext *yadif, const AVFrame *a, const AVFrame *b)
{
    int i;
    for (i = 0; i < yadif->csp->nb_components; i++)
        if (a->linesize[i] != b->linesize[i])
            return 1;
    return 0;
}
