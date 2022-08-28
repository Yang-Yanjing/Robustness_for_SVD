static void *resample_child_next(void *obj, void *prev)
{
    ResampleContext *s = obj;
    return prev ? NULL : s->avr;
}
