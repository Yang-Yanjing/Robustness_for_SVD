 */
static int Open (vlc_object_t *obj, const struct gl_api *api)
{
    vlc_gl_t *gl = (vlc_gl_t *)obj;
    vout_window_t *wnd = gl->surface;
    union {
        void *ext_platform;
        EGLNativeWindowType native;
    } window;
#ifdef EGL_EXT_platform_base
    PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC createSurface = NULL;
#endif
    vlc_gl_sys_t *sys = malloc(sizeof (*sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    gl->sys = sys;
    sys->display = EGL_NO_DISPLAY;
    sys->surface = EGL_NO_SURFACE;
#ifdef USE_PLATFORM_X11
    sys->x11 = NULL;
    if (wnd->type != VOUT_WINDOW_TYPE_XID || !vlc_xlib_init(obj))
        goto error;
    sys->x11 = XOpenDisplay(wnd->display.x11);
    if (sys->x11 == NULL)
        goto error;
    int snum;
    {
        XWindowAttributes wa;
        if (!XGetWindowAttributes(sys->x11, wnd->handle.xid, &wa))
            goto error;
        snum = XScreenNumberOfScreen(wa.screen);
    }
# ifdef EGL_EXT_platform_x11
    if (CheckClientExt("EGL_EXT_platform_x11"))
    {
        PFNEGLGETPLATFORMDISPLAYEXTPROC getDisplay;
        const EGLint attrs[] = {
            EGL_PLATFORM_X11_SCREEN_EXT, snum,
            EGL_NONE
        };
        getDisplay = (PFNEGLGETPLATFORMDISPLAYEXTPROC)
            eglGetProcAddress("eglGetPlatformDisplayEXT");
        createSurface = (PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC)
            eglGetProcAddress("eglCreatePlatformWindowSurfaceEXT");
        sys->display = getDisplay(EGL_PLATFORM_X11_EXT, sys->x11, attrs);
        window.ext_platform = &wnd->handle.xid;
    }
    else
# endif
    {
# if USE_DEFAULT_PLATFORM
        if (snum == XDefaultScreen(sys->x11))
        {
            sys->display = eglGetDisplay(sys->x11);
            window.native = wnd->handle.xid;
        }
# endif
    }
#elif defined (USE_PLATFORM_WIN32)
    if (wnd->type != VOUT_WINDOW_TYPE_HWND)
        goto error;
# if USE_DEFAULT_PLATFORM
    sys->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    window.native = wnd->handle.hwnd;
# endif
#elif defined (USE_PLATFORM_ANDROID)
    if (wnd->type != VOUT_WINDOW_TYPE_ANDROID_NATIVE)
        goto error;
# if USE_DEFAULT_PLATFORM
    sys->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    window.native = wnd->handle.anativewindow;
# endif
#endif
    if (sys->display == EGL_NO_DISPLAY)
        goto error;
    /* Initialize EGL display */
    EGLint major, minor;
    if (eglInitialize(sys->display, &major, &minor) != EGL_TRUE)
        goto error;
    msg_Dbg(obj, "EGL version %s by %s",
            eglQueryString(sys->display, EGL_VERSION),
            eglQueryString(sys->display, EGL_VENDOR));
    const char *ext = eglQueryString(sys->display, EGL_EXTENSIONS);
    if (*ext)
        msg_Dbg(obj, " extensions: %s", ext);
    if (major != 1 || minor < api->min_minor
     || !CheckAPI(sys->display, api->name))
    {
        msg_Err(obj, "cannot select %s API", api->name);
        goto error;
    }
    const EGLint conf_attr[] = {
        EGL_RED_SIZE, 5,
        EGL_GREEN_SIZE, 5,
        EGL_BLUE_SIZE, 5,
        EGL_RENDERABLE_TYPE, api->render_bit,
        EGL_NONE
    };
    EGLConfig cfgv[1];
    EGLint cfgc;
    if (eglChooseConfig(sys->display, conf_attr, cfgv, 1, &cfgc) != EGL_TRUE
     || cfgc == 0)
    {
        msg_Err (obj, "cannot choose EGL configuration");
        goto error;
    }
    /* Create a drawing surface */
#ifdef EGL_EXT_platform_base
    if (createSurface != NULL)
        sys->surface = createSurface(sys->display, cfgv[0],
                                     window.ext_platform, NULL);
    else
#endif
        sys->surface = eglCreateWindowSurface(sys->display, cfgv[0],
                                              window.native, NULL);
    if (sys->surface == EGL_NO_SURFACE)
    {
        msg_Err (obj, "cannot create EGL window surface");
        goto error;
    }
    if (eglBindAPI (api->api) != EGL_TRUE)
    {
        msg_Err (obj, "cannot bind EGL API");
        goto error;
    }
    EGLContext ctx = eglCreateContext(sys->display, cfgv[0], EGL_NO_CONTEXT,
                                      api->attr);
    if (ctx == EGL_NO_CONTEXT)
    {
        msg_Err (obj, "cannot create EGL context");
        goto error;
    }
    sys->context = ctx;
    /* Initialize OpenGL callbacks */
    gl->makeCurrent = MakeCurrent;
    gl->releaseCurrent = ReleaseCurrent;
    gl->swap = SwapBuffers;
    gl->getProcAddress = GetSymbol;
    gl->lock = NULL;
    gl->unlock = NULL;
    return VLC_SUCCESS;
error:
    Close (obj);
    return VLC_EGENERIC;
}
