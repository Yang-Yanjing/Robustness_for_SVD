 *****************************************************************************/
void ClosePacketizer( vlc_object_t *p_this )
{
    decoder_t     *p_dec = (decoder_t*)p_this;
    avcodec_free_context( &p_dec->p_sys->p_codec_ctx );
    av_parser_close( p_dec->p_sys->p_parser_ctx );
    es_format_Clean( &p_dec->fmt_out );
    free( p_dec->p_sys );
}
