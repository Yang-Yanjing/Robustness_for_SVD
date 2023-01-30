 */
static int Open(vout_window_t *wnd, const vout_window_cfg_t *cfg)
{
    vout_window_sys_t *p_sys = malloc(sizeof (*p_sys));
    if (p_sys == NULL)
        return VLC_ENOMEM;
    p_sys->p_library = LoadNativeWindowAPI(&p_sys->native_window);
    if (p_sys->p_library == NULL)
    {
        free(p_sys);
        return VLC_EGENERIC;
    }
    // Create the native window by first getting the Java surface.
    jobject javaSurface = jni_LockAndGetAndroidJavaSurface();
    if (javaSurface == NULL)
        goto error;
    JNIEnv *p_env;
    jni_attach_thread(&p_env, THREAD_NAME);
    p_sys->window = p_sys->native_window.winFromSurface(p_env, javaSurface); // ANativeWindow_fromSurface call.
    jni_detach_thread();
    jni_UnlockAndroidSurface();
    if (p_sys->window == NULL)
        goto error;
    wnd->handle.anativewindow = p_sys->window;
    wnd->control = Control;
    wnd->sys = p_sys;
    // Set the Java surface size.
    jni_SetAndroidSurfaceSize(cfg->width, cfg->height, cfg->width, cfg->height, 1, 1);
    return VLC_SUCCESS;
error:
    dlclose(p_sys->p_library);
    free(p_sys);
    return VLC_EGENERIC;
}
