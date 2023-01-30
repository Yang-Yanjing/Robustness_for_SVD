}
static bool EsIsSelected( es_out_id_t *es )
{
    if( es->p_master )
    {
        bool b_decode = false;
        if( es->p_master->p_dec )
        {
            int i_channel = EsOutGetClosedCaptionsChannel( es->fmt.i_codec );
            if( i_channel != -1 )
                input_DecoderGetCcState( es->p_master->p_dec, &b_decode, i_channel );
        }
        return b_decode;
    }
    else
    {
        return es->p_dec != NULL;
    }
}
