 */
static void GetOutputFormat( filter_t *p_filter,
                      video_format_t *p_dst, const video_format_t *p_src )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    *p_dst = *p_src;
    if( p_sys->b_half_height )
    {
        p_dst->i_height /= 2;
        p_dst->i_visible_height /= 2;
        p_dst->i_y_offset /= 2;
        p_dst->i_sar_den *= 2;
    }
    if( p_sys->b_double_rate )
    {
        p_dst->i_frame_rate *= 2;
    }
    if( p_sys->i_mode == DEINTERLACE_PHOSPHOR  &&
        2 * p_sys->chroma->p[1].h.num == p_sys->chroma->p[1].h.den &&
        2 * p_sys->chroma->p[2].h.num == p_sys->chroma->p[2].h.den &&
        p_sys->phosphor.i_chroma_for_420 == PC_UPCONVERT )
    {
        p_dst->i_chroma = p_src->i_chroma == VLC_CODEC_J420 ? VLC_CODEC_J422 :
                                                              VLC_CODEC_I422;
    }
    else
    {
        p_dst->i_chroma = p_src->i_chroma;
    }
}
