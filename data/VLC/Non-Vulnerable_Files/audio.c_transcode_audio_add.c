}
bool transcode_audio_add( sout_stream_t *p_stream, es_format_t *p_fmt, 
            sout_stream_id_sys_t *id )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    msg_Dbg( p_stream,
             "creating audio transcoding from fcc=`%4.4s' to fcc=`%4.4s'",
             (char*)&p_fmt->i_codec, (char*)&p_sys->i_acodec );
    /* Complete destination format */
    id->p_encoder->fmt_out.i_codec = p_sys->i_acodec;
    id->p_encoder->fmt_out.audio.i_rate = p_sys->i_sample_rate > 0 ?
        p_sys->i_sample_rate : p_fmt->audio.i_rate;
    id->p_encoder->fmt_out.i_bitrate = p_sys->i_abitrate;
    id->p_encoder->fmt_out.audio.i_bitspersample =
        p_fmt->audio.i_bitspersample;
    id->p_encoder->fmt_out.audio.i_channels = p_sys->i_channels > 0 ?
        p_sys->i_channels : p_fmt->audio.i_channels;
    id->p_encoder->fmt_in.audio.i_original_channels =
    id->p_encoder->fmt_out.audio.i_original_channels =
        id->p_decoder->fmt_out.audio.i_physical_channels;
    id->p_encoder->fmt_in.audio.i_physical_channels =
    id->p_encoder->fmt_out.audio.i_physical_channels =
            pi_channels_maps[id->p_encoder->fmt_out.audio.i_channels];
    /* Build decoder -> filter -> encoder chain */
    if( transcode_audio_new( p_stream, id ) == VLC_EGENERIC )
    {
        msg_Err( p_stream, "cannot create audio chain" );
        return false;
    }
    /* Open output stream */
    id->id = sout_StreamIdAdd( p_stream->p_next, &id->p_encoder->fmt_out );
    id->b_transcode = true;
    if( !id->id )
    {
        transcode_audio_close( id );
        return false;
    }
    /* Reinit encoder again later on, when all information from decoders
     * is available. */
    if( id->p_encoder->p_module )
    {
        module_unneed( id->p_encoder, id->p_encoder->p_module );
        id->p_encoder->p_module = NULL;
        if( id->p_encoder->fmt_out.p_extra )
        {
            free( id->p_encoder->fmt_out.p_extra );
            id->p_encoder->fmt_out.p_extra = NULL;
            id->p_encoder->fmt_out.i_extra = 0;
        }
        if( id->p_af_chain != NULL )
            aout_FiltersDelete( (vlc_object_t *)NULL, id->p_af_chain );
        id->p_af_chain = NULL;
    }
    return true;
}
