 */
static void Close(vlc_object_t *object)
{
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys = vd->sys;
    if (sys->pool)
        picture_pool_Delete(sys->pool);
    if (sys->dither)
        cucul_free_dither(sys->dither);
    caca_free_display(sys->dp);
    cucul_free_canvas(sys->cv);
#if defined(_WIN32)
    FreeConsole();
#endif
    free(sys);
}
