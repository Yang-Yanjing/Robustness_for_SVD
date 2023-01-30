}
int filter_Blend( filter_t *p_blend,
                  picture_t *p_dst, int i_dst_x, int i_dst_y,
                  const picture_t *p_src, int i_alpha )
{
    if( !p_blend->p_module )
        return VLC_EGENERIC;
    p_blend->pf_video_blend( p_blend, p_dst, p_src, i_dst_x, i_dst_y, i_alpha );
    return VLC_SUCCESS;
}
