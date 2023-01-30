}
static void DirectXCloseDisplay(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    if (sys->clipper != NULL)
        IDirectDrawClipper_Release(sys->clipper);
    if (sys->display != NULL)
        IDirectDrawSurface2_Release(sys->display);
    sys->clipper = NULL;
    sys->display = NULL;
}
