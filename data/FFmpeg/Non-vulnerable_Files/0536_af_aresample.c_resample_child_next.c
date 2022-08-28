static void *resample_child_next(void *obj, void *prev)
{
    AResampleContext *s = obj;
    return prev ? NULL : s->swr;
}
