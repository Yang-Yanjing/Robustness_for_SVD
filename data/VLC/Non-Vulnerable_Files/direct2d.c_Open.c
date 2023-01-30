 */
static int Open(vlc_object_t *object)
{
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys;
    vd->sys = sys = calloc(1, sizeof(*sys));
    if (!sys)
        return VLC_ENOMEM;
    sys->d2_render_target = NULL;
    sys->d2_dll = LoadLibrary(TEXT("D2D1.DLL"));
    if (!sys->d2_dll) {
        if (object->b_force)
            msg_Err(vd, "Cannot load D2D1.DLL, aborting");
        goto error;
    }
    D2D1_FACTORY_OPTIONS fo = {
        D2D1_DEBUG_LEVEL_NONE
    };
    HRESULT (WINAPI *D2D1CreateFactory)(D2D1_FACTORY_TYPE, REFIID,
                                        const D2D1_FACTORY_OPTIONS *,
                                        void **);
    D2D1CreateFactory = (void *)GetProcAddress(sys->d2_dll,
                                               "D2D1CreateFactory");
    if (!D2D1CreateFactory) {
        msg_Err(vd,
                "Cannot locate reference to a D2D1CreateFactory ABI in D2D1.DLL");
        goto error;
    }
#ifndef NDEBUG
    msg_Dbg(vd, "D2D1.DLL loaded");
#endif
    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                   (REFIID)&IID_ID2D1Factory,
                                   &fo,
                                   (void **)&sys->d2_factory);
    if (hr != S_OK) {
        msg_Err(vd, "Cannot create Direct2D factory (hr = 0x%x)!",
                (unsigned)hr);
        goto error;
    }
    if (CommonInit(vd))
        goto error;
    if (D2D_CreateRenderTarget(vd) != VLC_SUCCESS)
        goto error;
    vout_display_info_t info = vd->info;
    info.is_slow              = false;
    info.has_double_click     = true;
    info.has_hide_mouse       = false;
    info.has_pictures_invalid = false;
    vd->info = info;
    vd->fmt.i_chroma = VLC_CODEC_RGB32; /* masks change this to BGR32 for ID2D1Bitmap */
    vd->fmt.i_rmask  = 0x0000ff00;
    vd->fmt.i_gmask  = 0x00ff0000;
    vd->fmt.i_bmask  = 0xff000000;
    vd->pool    = Pool;
    vd->prepare = Prepare;
    vd->display = Display;
    vd->manage  = Manage;
    vd->control = Control;
    EventThreadUpdateTitle(sys->event, VOUT_TITLE " (Direct2D output)");
#ifndef NDEBUG
    msg_Dbg(vd, "Ready");
#endif
    return VLC_SUCCESS;
error:
    Close(VLC_OBJECT(vd));
    return VLC_EGENERIC;
}
