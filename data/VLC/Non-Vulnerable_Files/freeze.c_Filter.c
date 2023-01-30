 */
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic_in ) {
    if( !p_pic_in || !p_filter) return NULL;
    filter_sys_t *p_sys = p_filter->p_sys;
    picture_t *p_pic_out = filter_NewPicture( p_filter );
    if( unlikely(!p_pic_out) ) {
        picture_Release( p_pic_in );
        return NULL;
    }
   /*
    * allocate data
    */
    if ( unlikely(!p_sys->b_init) )
        if (freeze_allocate_data( p_filter, p_pic_in ) != VLC_SUCCESS)
        {
            picture_Release( p_pic_in );
            return NULL;
        }
    p_sys->b_init = true;
   /*
    * preset output pic: raw copy src to dst
    */
    picture_CopyPixels(p_pic_out, p_pic_in);
   /*
    * cache original pict pixels selected with mouse pointer
    */
    for ( int32_t i_p = 0; i_p < p_sys->i_planes; i_p++ )
        for ( int32_t i_r = 0; i_r < p_sys->i_height[i_p]; i_r++ )
            for ( int32_t i_c = 0; i_c < p_sys->i_width[i_p]; i_c++ )
            {
                uint32_t i_Yr = i_r * p_sys->i_height[Y_PLANE]
                              / p_sys->i_height[i_p];
                uint32_t i_Yc = i_c * p_sys->i_width[Y_PLANE]
                              / p_sys->i_width[i_p];
                if ( p_sys->pb_update_cache[i_Yr][i_Yc] )
                    p_sys->pi_freezed_picture[i_p][i_r][i_c]
                        = p_pic_in->p[i_p].p_pixels[i_r*p_pic_out->p[i_p].i_pitch
                        + i_c*p_pic_out->p[i_p].i_pixel_pitch];
            }
   /*
    * countdown freezed pixel delay & reset pb_update_cache flag
    */
    for ( int32_t i_Yr = 0; i_Yr < p_sys->i_height[Y_PLANE]; i_Yr++)
        for ( int32_t i_Yc = 0; i_Yc < p_sys->i_width[Y_PLANE]; i_Yc++)
        {
            if ( p_sys->pi_freezing_countdown[i_Yr][i_Yc] > 0 )
                 p_sys->pi_freezing_countdown[i_Yr][i_Yc]--;
            p_sys->pb_update_cache[i_Yr][i_Yc] = false;
        }
   /*
    * apply filter: draw freezed pixels over current picture
    */
    for ( int32_t i_p = 0; i_p < p_sys->i_planes; i_p++ )
        for ( int32_t i_r = 0; i_r < p_sys->i_height[i_p]; i_r++ )
            for ( int32_t i_c = 0; i_c < p_sys->i_width[i_p]; i_c++ )
            {
                uint32_t i_Yr = i_r * p_sys->i_height[Y_PLANE]
                              / p_sys->i_height[i_p];
                uint32_t i_Yc = i_c * p_sys->i_width[Y_PLANE]
                              / p_sys->i_width[i_p];
                if ( p_sys->pi_freezing_countdown[i_Yr][i_Yc] > 0 )
                    p_pic_out->p[i_p].p_pixels[i_r * p_pic_out->p[i_p].i_pitch
                        + i_c * p_pic_out->p[i_p].i_pixel_pitch]
                        = p_sys->pi_freezed_picture[i_p][i_r][i_c];
            }
    return CopyInfoAndRelease( p_pic_out, p_pic_in );
}
