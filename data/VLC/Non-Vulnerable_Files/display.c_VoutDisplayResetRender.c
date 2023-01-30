}
static void VoutDisplayResetRender(vout_display_t *vd)
{
    VoutDisplayDestroyRender(vd);
    VoutDisplayCreateRender(vd);
}
