}
static bool EsOutIsExtraBufferingAllowed( es_out_t *out )
{
    es_out_sys_t *p_sys = out->p_sys;
    size_t i_size = 0;
    for( int i = 0; i < p_sys->i_es; i++ )
    {
        es_out_id_t *p_es = p_sys->es[i];
        if( p_es->p_dec )
            i_size += input_DecoderGetFifoSize( p_es->p_dec );
        if( p_es->p_dec_record )
            i_size += input_DecoderGetFifoSize( p_es->p_dec_record );
    }
    //msg_Info( out, "----- EsOutIsExtraBufferingAllowed =% 5d KiB -- ", i_size / 1024 );
    /* TODO maybe we want to be able to tune it ? */
#if defined(OPTIMIZE_MEMORY)
    const size_t i_level_high = 512*1024;  /* 0.5 MiB */
#else
    const size_t i_level_high = 10*1024*1024; /* 10 MiB */
#endif
    return i_size < i_level_high;
}
