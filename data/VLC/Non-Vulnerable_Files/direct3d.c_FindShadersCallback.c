/* Populate the list of available shader techniques in the options */
static int FindShadersCallback(vlc_object_t *object, const char *name,
                               char ***values, char ***descs)
{
    VLC_UNUSED(object);
    VLC_UNUSED(name);
    enum_context_t ctx = { NULL, NULL, 0 };
    ListShaders(&ctx);
    *values = ctx.values;
    *descs = ctx.descs;
    return ctx.count;
}
