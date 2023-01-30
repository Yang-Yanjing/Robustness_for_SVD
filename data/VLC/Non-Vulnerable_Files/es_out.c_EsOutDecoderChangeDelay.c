}
static void EsOutDecoderChangeDelay( es_out_t *out, es_out_id_t *p_es )
{
    es_out_sys_t *p_sys = out->p_sys;
    mtime_t i_delay = 0;
    if( p_es->fmt.i_cat == AUDIO_ES )
        i_delay = p_sys->i_audio_delay;
    else if( p_es->fmt.i_cat == SPU_ES )
        i_delay = p_sys->i_spu_delay;
    else
        return;
    if( p_es->p_dec )
        input_DecoderChangeDelay( p_es->p_dec, i_delay );
    if( p_es->p_dec_record )
        input_DecoderChangeDelay( p_es->p_dec_record, i_delay );
}
