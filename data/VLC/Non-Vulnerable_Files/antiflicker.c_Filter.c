 *****************************************************************************/
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic )
{
    if( !p_pic ) return NULL;
    picture_t *p_outpic = filter_NewPicture( p_filter );
    if( !p_outpic )
    {
        picture_Release( p_pic );
        return NULL;
    }
    /****************** Get variables *************************/
    int i_window_size = atomic_load( &p_filter->p_sys->i_window_size );
    int i_softening = atomic_load( &p_filter->p_sys->i_softening );
    uint8_t *p_yplane_in = p_pic->p[Y_PLANE].p_pixels;
    uint8_t *p_yplane_out = p_outpic->p[Y_PLANE].p_pixels;
    bool scene_changed = false;
    int i_num_lines = p_pic->p[Y_PLANE].i_visible_lines;
    int i_num_cols = p_pic->p[Y_PLANE].i_visible_pitch;
    int i_in_pitch = p_pic->p[Y_PLANE].i_pitch;
    int i_out_pitch = p_outpic->p[Y_PLANE].i_pitch;
    /******** Get the luminance average for the current picture ********/
    int lum_avg = GetLuminanceAvg(p_pic);
    /*Identify as scene change if the luminance average deviates
     more than the threshold value or if it is the first frame*/
    if( abs(lum_avg - p_filter->p_sys->
        ia_luminance_data[i_window_size - 1]) > SCENE_CHANGE_THRESHOLD
        || p_filter->p_sys->ia_luminance_data[i_window_size - 1] == 256)
    {
        scene_changed = true;
    }
    if ( scene_changed )
    {
        //reset the luminance data
        for (int i = 0; i < i_window_size; ++i)
            p_filter->p_sys->ia_luminance_data[i] = lum_avg;
        plane_CopyPixels( &p_outpic->p[Y_PLANE], &p_pic->p[Y_PLANE] );
    }
    else
    {
        /******* Compute the adjustment factor using moving average ********/
        for (int i = 0; i < i_window_size-1 ; ++i)
            p_filter->p_sys->ia_luminance_data[i] =
                           p_filter->p_sys->ia_luminance_data[i+1];
        p_filter->p_sys->ia_luminance_data[i_window_size - 1] = lum_avg;
        float scale = 1.0;
        if (lum_avg > 0)
        {
             float filt = 0;
             for (int i = 0; i < i_window_size; i++)
                  filt += (float) p_filter->p_sys->ia_luminance_data[i];
             scale = filt/(i_window_size*lum_avg);
        }
        /******* Apply the adjustment factor to each pixel on Y_PLANE ********/
        uint8_t shift = 8;
        int scale_num = __MIN(scale,255) * ( 1 << shift );
        for( int i_line = 0 ; i_line < i_num_lines ; i_line++ )
        {
            for( int i_col = 0; i_col < i_num_cols  ; i_col++ )
            {
                uint8_t pixel_data = p_yplane_in[i_line*i_in_pitch+i_col];
                int pixel_val = ( scale_num * pixel_data +
                       (1<<(shift -1)) ) >> shift;
                p_yplane_out[i_line*i_out_pitch+i_col] =
                       (pixel_val>255) ? 255:pixel_val;
            }
        }
    }
    /***************** Copy the UV plane as such *****************************/
    plane_CopyPixels( &p_outpic->p[U_PLANE], &p_pic->p[U_PLANE] );
    plane_CopyPixels( &p_outpic->p[V_PLANE], &p_pic->p[V_PLANE] );
    if (scene_changed || i_softening == 0)
    {
       return CopyInfoAndRelease( p_outpic, p_pic );
    }
    /******* Temporal softening phase. Adapted from code by Steven Don ******/
    uint8_t *p_yplane_out_old = p_filter->p_sys->p_old_data;
    int i_video_width = p_filter->fmt_in.video.i_width;
    for( int i_line = 0 ; i_line < i_num_lines ; i_line++ )
    {
        for( int i_col = 0; i_col < i_num_cols  ; i_col++ )
        {
            uint8_t pixel_data = p_yplane_out[i_line*i_out_pitch+i_col];
            uint8_t pixel_old = p_yplane_out_old[i_line*i_video_width+i_col];
            int diff = abs(pixel_data - pixel_old);
            if (diff < i_softening)
            {
                if (diff > (i_softening >> 1))
                {
                    p_yplane_out_old[i_line*i_video_width+i_col] =
                        ((pixel_data * 2) + pixel_old) /3;
                }
            }
            else
            {
                p_yplane_out_old[i_line*i_video_width+i_col] = pixel_data;
            }
            p_yplane_out[i_line*i_out_pitch+i_col] =
                p_yplane_out_old[i_line*i_video_width+i_col];
        }
    }
    return CopyInfoAndRelease( p_outpic, p_pic );
}
