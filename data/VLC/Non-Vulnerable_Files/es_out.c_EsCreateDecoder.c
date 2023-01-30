}
static void EsCreateDecoder( es_out_t *out, es_out_id_t *p_es )
{
    es_out_sys_t   *p_sys = out->p_sys;
    input_thread_t *p_input = p_sys->p_input;
    p_es->p_dec = input_DecoderNew( p_input, &p_es->fmt, p_es->p_pgrm->p_clock, p_input->p->p_sout );
    if( p_es->p_dec )
    {
        if( p_sys->b_buffering )
            input_DecoderStartWait( p_es->p_dec );
        if( !p_es->p_master && p_sys->p_sout_record )
        {
            p_es->p_dec_record = input_DecoderNew( p_input, &p_es->fmt, p_es->p_pgrm->p_clock, p_sys->p_sout_record );
            if( p_es->p_dec_record && p_sys->b_buffering )
                input_DecoderStartWait( p_es->p_dec_record );
        }
    }
    EsOutDecoderChangeDelay( out, p_es );
}
