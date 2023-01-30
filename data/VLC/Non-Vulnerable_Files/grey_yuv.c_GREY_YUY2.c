 *****************************************************************************/
static void GREY_YUY2( filter_t *p_filter, picture_t *p_source,
                                           picture_t *p_dest )
{
    uint8_t *p_in = p_source->p->p_pixels;
    uint8_t *p_out = p_dest->p->p_pixels;
    int i_x, i_y;
    const int i_source_margin = p_source->p->i_pitch
                                 - p_source->p->i_visible_pitch;
    const int i_dest_margin = p_dest->p->i_pitch
                               - p_dest->p->i_visible_pitch;
    for( i_y = p_filter->fmt_out.video.i_height; i_y-- ; )
    {
        for( i_x = p_filter->fmt_out.video.i_width / 8; i_x-- ; )
        {
            *p_out++ = *p_in++; *p_out++ = 0x80;
            *p_out++ = *p_in++; *p_out++ = 0x80;
            *p_out++ = *p_in++; *p_out++ = 0x80;
            *p_out++ = *p_in++; *p_out++ = 0x80;
            *p_out++ = *p_in++; *p_out++ = 0x80;
            *p_out++ = *p_in++; *p_out++ = 0x80;
            *p_out++ = *p_in++; *p_out++ = 0x80;
            *p_out++ = *p_in++; *p_out++ = 0x80;
        }
        for( i_x = (p_filter->fmt_out.video.i_width % 8) / 2; i_x-- ; )
        {
            *p_out++ = *p_in++; *p_out++ = 0x80;
            *p_out++ = *p_in++; *p_out++ = 0x80;
        }
        p_in += i_source_margin;
        p_out += i_dest_margin;
    }
}
