}
inline static picture_t *video_new_buffer_filter( filter_t *p_filter )
{
    return video_new_buffer( VLC_OBJECT( p_filter ),
                             (decoder_owner_sys_t *)p_filter->p_owner,
                             &p_filter->fmt_out );
}
