void *av_opt_child_next(void *obj, void *prev)
{
    const AVClass *c = *(AVClass**)obj;
    if (c->child_next)
        return c->child_next(obj, prev);
    return NULL;
}
