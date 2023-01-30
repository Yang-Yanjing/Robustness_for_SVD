}
static sout_stream_id_sys_t *AddAudio( sout_stream_t *p_stream, es_format_t *p_fmt )
{
    char* psz_tmp;
    sout_stream_id_sys_t* id;
    int i_bits_per_sample = aout_BitsPerSample( p_fmt->i_codec );
    if( !i_bits_per_sample )
    {
        msg_Err( p_stream, "Smem does only support raw audio format" );
        return NULL;
    }
    id = calloc( 1, sizeof( sout_stream_id_sys_t ) );
    if( !id )
        return NULL;
    psz_tmp = var_GetString( p_stream, SOUT_PREFIX_AUDIO "data" );
    id->p_data = (void *)( intptr_t )atoll( psz_tmp );
    free( psz_tmp );
    id->format = p_fmt;
    id->format->audio.i_bitspersample = i_bits_per_sample;
    return id;
}
