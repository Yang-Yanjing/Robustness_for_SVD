 */
static void oldmovie_shutter_effect( filter_t *p_filter, picture_t *p_pic_out ) {
    filter_sys_t *p_sys = p_filter->p_sys;
#define SHUTTER_FREQ      2
#define SHUTTER_SPEED     25
#define SHUTTER_HEIGHT    1.5
#define SHUTTER_INTENSITY 50
#define SUB_FRAME (p_sys->i_cur_time % (TIME_UNIT_PER_S / SHUTTER_FREQ))
   /*
    * depending on current time: define shutter location on picture
    */
    int32_t i_shutter_sup = VLC_CLIP((int64_t)SUB_FRAME
                                      * p_pic_out->p[Y_PLANE].i_visible_lines
                                      * SHUTTER_SPEED / TIME_UNIT_PER_S,
                                      0, p_pic_out->p[Y_PLANE].i_visible_lines);
    int32_t i_shutter_inf = VLC_CLIP((int64_t)SUB_FRAME
                                      * p_pic_out->p[Y_PLANE].i_visible_lines
                                      * SHUTTER_SPEED / TIME_UNIT_PER_S
                                      - SHUTTER_HEIGHT * p_pic_out->p[Y_PLANE].i_visible_lines,
                                      0, p_pic_out->p[Y_PLANE].i_visible_lines);
    int32_t i_width = p_pic_out->p[Y_PLANE].i_visible_pitch
                    / p_pic_out->p[Y_PLANE].i_pixel_pitch;
   /*
    * darken pixels currently occulted by shutter
    */
    for ( int32_t i_y = i_shutter_inf; i_y < i_shutter_sup; i_y++ )
        for ( int32_t i_x = 0; i_x < i_width; i_x++ )
            DARKEN_PIXEL( i_x, i_y, SHUTTER_INTENSITY, &p_pic_out->p[Y_PLANE] );
}
