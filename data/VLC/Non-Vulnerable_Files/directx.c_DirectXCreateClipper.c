 */
static void DirectXCreateClipper(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    HRESULT hr;
    /* Create the clipper */
    hr = IDirectDraw2_CreateClipper(sys->ddobject, 0, &sys->clipper, NULL);
    if (hr != DD_OK) {
        msg_Warn(vd, "cannot create clipper (error %li)", hr);
        goto error;
    }
    /* Associate the clipper to the window */
    hr = IDirectDrawClipper_SetHWnd(sys->clipper, 0, sys->hvideownd);
    if (hr != DD_OK) {
        msg_Warn(vd, "cannot attach clipper to window (error %li)", hr);
        goto error;
    }
    /* associate the clipper with the surface */
    hr = IDirectDrawSurface_SetClipper(sys->display, sys->clipper);
    if (hr != DD_OK)
    {
        msg_Warn(vd, "cannot attach clipper to surface (error %li)", hr);
        goto error;
    }
    return;
error:
    if (sys->clipper)
        IDirectDrawClipper_Release(sys->clipper);
    sys->clipper = NULL;
}
