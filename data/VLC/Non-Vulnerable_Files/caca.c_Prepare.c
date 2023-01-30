 * Prepare a picture for display */
static void Prepare(vout_display_t *vd, picture_t *picture, subpicture_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    if (!sys->dither) {
        /* Create the libcaca dither object */
        sys->dither = cucul_create_dither(32,
                                            vd->source.i_visible_width,
                                            vd->source.i_visible_height,
                                            picture->p[0].i_pitch,
                                            vd->fmt.i_rmask,
                                            vd->fmt.i_gmask,
                                            vd->fmt.i_bmask,
                                            0x00000000);
        if (!sys->dither) {
            msg_Err(vd, "could not create libcaca dither object");
            return;
        }
    }
    vout_display_place_t place;
    Place(vd, &place);
    cucul_set_color_ansi(sys->cv, CUCUL_COLOR_DEFAULT, CUCUL_COLOR_BLACK);
    cucul_clear_canvas(sys->cv);
    const int crop_offset = vd->source.i_y_offset * picture->p->i_pitch +
                            vd->source.i_x_offset * picture->p->i_pixel_pitch;
    cucul_dither_bitmap(sys->cv, place.x, place.y,
                        place.width, place.height,
                        sys->dither,
                        &picture->p->p_pixels[crop_offset]);
    VLC_UNUSED(subpicture);
}
