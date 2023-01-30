}
static picture_t *FilterPacked( filter_t *p_filter, picture_t *p_pic )
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
    int i_y_offset, i_u_offset, i_v_offset;
    int i_ret = GetPackedYuvOffsets( p_filter->fmt_in.video.i_chroma,
                                     &i_y_offset, &i_u_offset, &i_v_offset );
    if( i_ret == VLC_EGENERIC )
    {
        picture_Release( p_pic );
        return NULL;
    }
    /*
     * Copy Y and do the U and V planes
     */
    int refu, refv, reflength;
    GetReference( &refu, &refv, &reflength, i_color );
    for( int y = 0; y < p_pic->p->i_visible_lines; y++ )
    {
        uint8_t *p_src = &p_pic->p->p_pixels[y * p_pic->p->i_pitch];
        uint8_t *p_dst = &p_outpic->p->p_pixels[y * p_outpic->p->i_pitch];
        for( int x = 0; x < p_pic->p->i_visible_pitch / 4; x++ )
        {
            p_dst[i_y_offset + 0] = p_src[i_y_offset + 0];
            p_dst[i_y_offset + 2] = p_src[i_y_offset + 2];
            if( IsSimilar( p_src[i_u_offset] - 0x80, p_src[i_v_offset] - 0x80,
                           refu, refv, reflength,
                           i_satthres, i_simthres ) )
            {
                p_dst[i_u_offset] = p_src[i_u_offset];
                p_dst[i_v_offset] = p_src[i_v_offset];
            }
            else
            {
                p_dst[i_u_offset] = 0x80;
                p_dst[i_v_offset] = 0x80;
            }
            p_dst += 4;
            p_src += 4;
        }
    }
    return CopyInfoAndRelease( p_outpic, p_pic );
}
