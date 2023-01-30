}
inline static picture_t *video_new_buffer_decoder( decoder_t *p_dec )
{
    return video_new_buffer( VLC_OBJECT( p_dec ),
                             (decoder_owner_sys_t *)p_dec->p_owner,
                             &p_dec->fmt_out );
}
