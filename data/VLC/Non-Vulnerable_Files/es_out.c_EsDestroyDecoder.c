}
static void EsDestroyDecoder( es_out_t *out, es_out_id_t *p_es )
{
    VLC_UNUSED(out);
    if( !p_es->p_dec )
        return;
    input_DecoderDelete( p_es->p_dec );
    p_es->p_dec = NULL;
    if( p_es->p_dec_record )
    {
        input_DecoderDelete( p_es->p_dec_record );
        p_es->p_dec_record = NULL;
    }
}
