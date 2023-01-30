 *****************************************************************************/
int OpenPacketizer( vlc_object_t *p_this )
{
    decoder_t     *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    /* Restrict to VP9 for now */
    if( p_dec->fmt_in.i_codec != VLC_CODEC_VP9 )
        return VLC_EGENERIC;
    unsigned i_avcodec_id;
    if( !GetFfmpegCodec( p_dec->fmt_in.i_codec, NULL, &i_avcodec_id, NULL ) )
        return VLC_EGENERIC;
    /* init avcodec */
    vlc_init_avcodec(p_this);
    /* It is less likely to have a parser than a codec, start by that */
    AVCodecParserContext * p_ctx = av_parser_init( i_avcodec_id );
    if( !p_ctx )
        return VLC_EGENERIC;
    AVCodec * p_codec = avcodec_find_decoder( i_avcodec_id );
    if( unlikely( !p_codec ) )
    {
        av_parser_close( p_ctx );
        return VLC_EGENERIC;
    }
    AVCodecContext * p_codec_ctx = avcodec_alloc_context3( p_codec );
    if( unlikely( !p_codec_ctx ) )
    {
        av_parser_close( p_ctx );
        return VLC_ENOMEM;
    }
    p_dec->p_sys = p_sys = malloc( sizeof(*p_sys) );
    if( unlikely( !p_sys ) )
    {
        avcodec_free_context( &p_codec_ctx );
        av_parser_close( p_ctx );
        return VLC_ENOMEM;
    }
    p_dec->pf_packetize = Packetize;
    p_sys->p_parser_ctx = p_ctx;
    p_sys->p_codec_ctx = p_codec_ctx;
    p_sys->i_offset = 0;
    es_format_Copy( &p_dec->fmt_out, &p_dec->fmt_in );
    return VLC_SUCCESS;
}
