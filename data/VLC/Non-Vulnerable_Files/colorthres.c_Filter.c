 *****************************************************************************/
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic )
{
    picture_t *p_outpic;
    filter_sys_t *p_sys = p_filter->p_sys;
    int i_simthres = atomic_load( &p_sys->i_simthres );
    int i_satthres = atomic_load( &p_sys->i_satthres );
    int i_color = atomic_load( &p_sys->i_color );
    if( !p_pic ) return NULL;
    p_outpic = filter_NewPicture( p_filter );
    if( !p_outpic )
    {
        picture_Release( p_pic );
        return NULL;
    }
    /* Copy the Y plane */
    plane_CopyPixels( &p_outpic->p[Y_PLANE], &p_pic->p[Y_PLANE] );
    /*
     * Do the U and V planes
     */
    int refu, refv, reflength;
    GetReference( &refu, &refv, &reflength, i_color );
    for( int y = 0; y < p_pic->p[U_PLANE].i_visible_lines; y++ )
    {
        uint8_t *p_src_u = &p_pic->p[U_PLANE].p_pixels[y * p_pic->p[U_PLANE].i_pitch];
        uint8_t *p_src_v = &p_pic->p[V_PLANE].p_pixels[y * p_pic->p[V_PLANE].i_pitch];
        uint8_t *p_dst_u = &p_outpic->p[U_PLANE].p_pixels[y * p_outpic->p[U_PLANE].i_pitch];
        uint8_t *p_dst_v = &p_outpic->p[V_PLANE].p_pixels[y * p_outpic->p[V_PLANE].i_pitch];
        for( int x = 0; x < p_pic->p[U_PLANE].i_visible_pitch; x++ )
        {
            if( IsSimilar( *p_src_u - 0x80, *p_src_v - 0x80,
                           refu, refv, reflength,
                           i_satthres, i_simthres ) )
            {
                *p_dst_u++ = *p_src_u;
                *p_dst_v++ = *p_src_v;
            }
            else
            {
                *p_dst_u++ = 0x80;
                *p_dst_v++ = 0x80;
            }
            p_src_u++;
            p_src_v++;
        }
    }
    return CopyInfoAndRelease( p_outpic, p_pic );
}
