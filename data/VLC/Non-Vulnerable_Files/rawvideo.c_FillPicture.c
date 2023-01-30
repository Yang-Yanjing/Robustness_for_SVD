 *****************************************************************************/
static void FillPicture( decoder_t *p_dec, block_t *p_block, picture_t *p_pic )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    const uint8_t *p_src = p_block->p_buffer;
    if( p_sys->b_invert )
        for( int i = 0; i < p_pic->i_planes; i++ )
        {
            uint8_t *p_dst = p_pic->p[i].p_pixels
                         + (p_pic->p[i].i_pitch * p_pic->p[i].i_visible_lines);
            for( int x = 0; x < p_pic->p[i].i_visible_lines; x++ )
            {
                p_dst -= p_pic->p[i].i_pitch;
                memcpy( p_dst, p_src, p_pic->p[i].i_visible_pitch );
                p_src += p_sys->pitches[i];
            }
            p_src += p_sys->pitches[i]
                   * (p_sys->lines[i] - p_pic->p[i].i_visible_lines);
        }
    else
        for( int i = 0; i < p_pic->i_planes; i++ )
        {
            uint8_t *p_dst = p_pic->p[i].p_pixels;
            for( int x = 0; x < p_pic->p[i].i_visible_lines; x++ )
            {
                memcpy( p_dst, p_src, p_pic->p[i].i_visible_pitch );
                p_src += p_sys->pitches[i];
                p_dst += p_pic->p[i].i_pitch;
            }
            p_src += p_sys->pitches[i]
                   * (p_sys->lines[i] - p_pic->p[i].i_visible_lines);
        }
}
