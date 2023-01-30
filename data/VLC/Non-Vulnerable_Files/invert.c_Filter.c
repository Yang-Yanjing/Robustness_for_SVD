 *****************************************************************************/
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic )
{
    picture_t *p_outpic;
    int i_index;
    int i_planes;
    if( !p_pic ) return NULL;
    p_outpic = filter_NewPicture( p_filter );
    if( !p_outpic )
    {
        msg_Warn( p_filter, "can't get output picture" );
        picture_Release( p_pic );
        return NULL;
    }
    if( p_pic->format.i_chroma == VLC_CODEC_YUVA )
    {
        /* We don't want to invert the alpha plane */
        i_planes = p_pic->i_planes - 1;
        memcpy(
            p_outpic->p[A_PLANE].p_pixels, p_pic->p[A_PLANE].p_pixels,
            p_pic->p[A_PLANE].i_pitch *  p_pic->p[A_PLANE].i_lines );
    }
    else
    {
        i_planes = p_pic->i_planes;
    }
    for( i_index = 0 ; i_index < i_planes ; i_index++ )
    {
        uint8_t *p_in, *p_in_end, *p_line_end, *p_out;
        p_in = p_pic->p[i_index].p_pixels;
        p_in_end = p_in + p_pic->p[i_index].i_visible_lines
                           * p_pic->p[i_index].i_pitch;
        p_out = p_outpic->p[i_index].p_pixels;
        for( ; p_in < p_in_end ; )
        {
            uint64_t *p_in64, *p_out64;
            p_line_end = p_in + p_pic->p[i_index].i_visible_pitch - 64;
            p_in64 = (uint64_t*)p_in;
            p_out64 = (uint64_t*)p_out;
            while( p_in64 < (uint64_t *)p_line_end )
            {
                /* Do 64 pixels at a time */
                *p_out64++ = ~*p_in64++; *p_out64++ = ~*p_in64++;
                *p_out64++ = ~*p_in64++; *p_out64++ = ~*p_in64++;
                *p_out64++ = ~*p_in64++; *p_out64++ = ~*p_in64++;
                *p_out64++ = ~*p_in64++; *p_out64++ = ~*p_in64++;
            }
            p_in = (uint8_t*)p_in64;
            p_out = (uint8_t*)p_out64;
            p_line_end += 64;
            for( ; p_in < p_line_end ; )
            {
                *p_out++ = ~( *p_in++ );
            }
            p_in += p_pic->p[i_index].i_pitch
                     - p_pic->p[i_index].i_visible_pitch;
            p_out += p_outpic->p[i_index].i_pitch
                     - p_outpic->p[i_index].i_visible_pitch;
        }
    }
    return CopyInfoAndRelease( p_outpic, p_pic );
}
