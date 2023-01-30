}
static decoder_t *CreateDecoder( vlc_object_t *p_this, video_format_t *fmt )
{
    decoder_t *p_dec;
    p_dec = vlc_custom_create( p_this, sizeof( *p_dec ), "image decoder" );
    if( p_dec == NULL )
        return NULL;
    p_dec->p_module = NULL;
    es_format_Init( &p_dec->fmt_in, VIDEO_ES, fmt->i_chroma );
    es_format_Init( &p_dec->fmt_out, VIDEO_ES, 0 );
    p_dec->fmt_in.video = *fmt;
    p_dec->b_pace_control = true;
    p_dec->pf_vout_buffer_new = video_new_buffer;
    p_dec->pf_vout_buffer_del = video_del_buffer;
    p_dec->pf_picture_link    = video_link_picture;
    p_dec->pf_picture_unlink  = video_unlink_picture;
    /* Find a suitable decoder module */
    p_dec->p_module = module_need( p_dec, "decoder", "$codec", false );
    if( !p_dec->p_module )
    {
        msg_Err( p_dec, "no suitable decoder module for fourcc `%4.4s'. "
                 "VLC probably does not support this image format.",
                 (char*)&p_dec->fmt_in.i_codec );
        DeleteDecoder( p_dec );
        return NULL;
    }
    return p_dec;
}
