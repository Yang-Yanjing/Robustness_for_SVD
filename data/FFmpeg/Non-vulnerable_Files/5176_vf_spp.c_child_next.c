static void *child_next(void *obj, void *prev)
{
    SPPContext *s = obj;
    return prev ? NULL : s->dct;
}
