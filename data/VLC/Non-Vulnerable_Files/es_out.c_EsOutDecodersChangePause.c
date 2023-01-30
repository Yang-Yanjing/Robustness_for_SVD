}
static void EsOutDecodersChangePause( es_out_t *out, bool b_paused, mtime_t i_date )
{
    es_out_sys_t *p_sys = out->p_sys;
    /* Pause decoders first */
    for( int i = 0; i < p_sys->i_es; i++ )
    {
        es_out_id_t *es = p_sys->es[i];
        if( es->p_dec )
        {
            input_DecoderChangePause( es->p_dec, b_paused, i_date );
            if( es->p_dec_record )
                input_DecoderChangePause( es->p_dec_record, b_paused, i_date );
        }
    }
}
