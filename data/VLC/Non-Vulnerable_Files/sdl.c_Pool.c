 */
static picture_pool_t *Pool(vout_display_t *vd, unsigned count)
{
    vout_display_sys_t *sys = vd->sys;
    VLC_UNUSED(count);
    if (!sys->pool) {
        picture_resource_t rsc;
        memset(&rsc, 0, sizeof(rsc));
        if (sys->overlay) {
            SDL_Overlay *ol = sys->overlay;
            for (int i = 0; i < ol->planes; i++) {
                rsc.p[i].p_pixels = ol->pixels[ i > 0 && sys->is_uv_swapped ? (3-i) : i];
                rsc.p[i].i_pitch  = ol->pitches[i > 0 && sys->is_uv_swapped ? (3-i) : i];
                rsc.p[i].i_lines  = ol->h;
                if (ol->format == SDL_YV12_OVERLAY ||
                    ol->format == SDL_IYUV_OVERLAY)
                    rsc.p[i].i_lines /= 2;
            }
        } else {
            const int x = sys->place.x;
            const int y = sys->place.y;
            SDL_Surface *sf = sys->display;
            SDL_FillRect(sf, NULL, 0);
            assert(x >= 0 && y >= 0);
            rsc.p[0].p_pixels = (uint8_t*)sf->pixels + y * sf->pitch + x * ((sf->format->BitsPerPixel + 7) / 8);
            rsc.p[0].i_pitch  = sf->pitch;
            rsc.p[0].i_lines  = vd->fmt.i_height;
        }
        picture_t *picture = picture_NewFromResource(&vd->fmt, &rsc);;
        if (!picture)
            return NULL;
        sys->pool = picture_pool_New(1, &picture);
    }
    return sys->pool;
}
