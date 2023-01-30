}
static void EsOutSetDelay( es_out_t *out, int i_cat, int64_t i_delay )
{
    es_out_sys_t *p_sys = out->p_sys;
    if( i_cat == AUDIO_ES )
        p_sys->i_audio_delay = i_delay;
    else if( i_cat == SPU_ES )
        p_sys->i_spu_delay = i_delay;
    for( int i = 0; i < p_sys->i_es; i++ )
        EsOutDecoderChangeDelay( out, p_sys->es[i] );
}
