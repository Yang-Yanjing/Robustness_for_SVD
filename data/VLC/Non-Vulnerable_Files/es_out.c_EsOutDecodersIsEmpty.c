}
static bool EsOutDecodersIsEmpty( es_out_t *out )
{
    es_out_sys_t      *p_sys = out->p_sys;
    if( p_sys->b_buffering && p_sys->p_pgrm )
    {
        EsOutDecodersStopBuffering( out, true );
        if( p_sys->b_buffering )
            return true;
    }
    for( int i = 0; i < p_sys->i_es; i++ )
    {
        es_out_id_t *es = p_sys->es[i];
        if( es->p_dec && !input_DecoderIsEmpty( es->p_dec ) )
            return false;
        if( es->p_dec_record && !input_DecoderIsEmpty( es->p_dec_record ) )
            return false;
    }
    return true;
}
