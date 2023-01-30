 */
static void Close(vlc_object_t *object)
{
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys = vd->sys;
    if (sys->pool)
        picture_pool_Delete(sys->pool);
    if (sys->overlay) {
        SDL_LockYUVOverlay(sys->overlay);
        SDL_FreeYUVOverlay(sys->overlay);
    }
    SDL_UnlockSurface (sys->display);
    SDL_FreeSurface(sys->display);
    vlc_mutex_lock(&sdl_lock);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    vlc_mutex_unlock(&sdl_lock);
    free(sys);
}
