}
static void DirectXCloseDDraw(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    if (sys->ddobject)
        IDirectDraw2_Release(sys->ddobject);
    sys->ddobject = NULL;
    free(sys->display_driver);
    sys->display_driver = NULL;
    sys->hmonitor = NULL;
}
