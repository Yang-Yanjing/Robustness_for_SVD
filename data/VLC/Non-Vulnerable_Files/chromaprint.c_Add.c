}
static sout_stream_id_sys_t *Add( sout_stream_t *p_stream, es_format_t *p_fmt )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    sout_stream_id_sys_t *id = NULL;
    if ( p_fmt->i_cat == AUDIO_ES && !p_sys->id )
    {
        if( p_fmt->i_codec != VLC_CODEC_S16N || p_fmt->audio.i_channels > 2 )
        {
            msg_Warn( p_stream, "bad input format: need s16l, 1 or 2 channels" );
            goto error;
        }
        id = malloc( sizeof( sout_stream_id_sys_t ) );
        if ( !id ) goto error;
        id->i_channels = p_fmt->audio.i_channels;
        id->i_samplerate = p_fmt->audio.i_rate;
        id->i_samples = p_sys->i_duration * id->i_samplerate;
        if ( !chromaprint_start( p_sys->p_chromaprint_ctx, p_fmt->audio.i_rate, id->i_channels ) )
        {
            msg_Err( p_stream, "Failed starting chromaprint on %uHz %uch samples",
                     p_fmt->audio.i_rate, id->i_channels );
            goto error;
        }
        else
        {
            p_sys->id = id;
            msg_Dbg( p_stream, "Starting chromaprint on %uHz %uch samples",
                     p_fmt->audio.i_rate, id->i_channels );
        }
        return id;
    }
error:
    free( id );
    return NULL;
}
