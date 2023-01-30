 */
int transcode_osd_new( sout_stream_t *p_stream, sout_stream_id_sys_t *id )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    id->p_decoder->fmt_in.i_cat = SPU_ES;
    id->p_encoder->fmt_out.psz_language = strdup( "osd" );
    if( p_sys->i_osdcodec != 0 || p_sys->psz_osdenc )
    {
        msg_Dbg( p_stream, "creating osdmenu transcoding from fcc=`%4.4s' "
                 "to fcc=`%4.4s'", (char*)&id->p_encoder->fmt_out.i_codec,
                 (char*)&p_sys->i_osdcodec );
        /* Complete destination format */
        id->p_encoder->fmt_out.i_codec = p_sys->i_osdcodec;
        /* Open encoder */
        es_format_Init( &id->p_encoder->fmt_in, id->p_decoder->fmt_in.i_cat,
                        VLC_CODEC_YUVA );
        id->p_encoder->fmt_in.psz_language = strdup( "osd" );
        id->p_encoder->p_cfg = p_sys->p_osd_cfg;
        id->p_encoder->p_module =
            module_need( id->p_encoder, "encoder", p_sys->psz_osdenc, true );
        if( !id->p_encoder->p_module )
        {
            msg_Err( p_stream, "cannot find spu encoder (%s)", p_sys->psz_osdenc );
            goto error;
        }
        /* open output stream */
        id->id = sout_StreamIdAdd( p_stream->p_next, &id->p_encoder->fmt_out );
        id->b_transcode = true;
        if( !id->id ) goto error;
    }
    else
    {
        msg_Dbg( p_stream, "not transcoding a stream (fcc=`%4.4s')",
                 (char*)&id->p_decoder->fmt_out.i_codec );
        id->id = sout_StreamIdAdd( p_stream->p_next, &id->p_decoder->fmt_out );
        id->b_transcode = false;
        if( !id->id ) goto error;
    }
    if( !p_sys->p_spu )
        p_sys->p_spu = spu_Create( p_stream );
    return VLC_SUCCESS;
 error:
    msg_Err( p_stream, "starting osd encoding thread failed" );
    if( id->p_encoder->p_module )
            module_unneed( id->p_encoder, id->p_encoder->p_module );
    p_sys->b_osd = false;
    return VLC_EGENERIC;
}
