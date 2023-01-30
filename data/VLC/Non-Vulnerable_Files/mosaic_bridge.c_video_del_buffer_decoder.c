}
inline static void video_del_buffer_decoder( decoder_t *p_this,
                                             picture_t *p_pic )
{
    VLC_UNUSED(p_this);
    picture_Release( p_pic );
}
