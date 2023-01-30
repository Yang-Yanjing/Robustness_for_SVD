 */
static void PictureDisplay(vout_display_t *vd, picture_t *p_pic, subpicture_t *p_subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    if (sys->overlay) {
        SDL_Rect disp;
        disp.x = sys->place.x;
        disp.y = sys->place.y;
        disp.w = sys->place.width;
        disp.h = sys->place.height;
        SDL_UnlockYUVOverlay(sys->overlay);
        SDL_DisplayYUVOverlay(sys->overlay , &disp);
        SDL_LockYUVOverlay(sys->overlay);
    } else {
        SDL_Flip(sys->display);
    }
    picture_Release(p_pic);
    VLC_UNUSED(p_subpicture);
}
