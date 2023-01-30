}
static void EsUnselect( es_out_t *out, es_out_id_t *es, bool b_update )
{
    es_out_sys_t   *p_sys = out->p_sys;
    input_thread_t *p_input = p_sys->p_input;
    if( !EsIsSelected( es ) )
    {
        msg_Warn( p_input, "ES 0x%x is already unselected", es->i_id );
        return;
    }
    if( es->p_master )
    {
        if( es->p_master->p_dec )
        {
            int i_channel = EsOutGetClosedCaptionsChannel( es->fmt.i_codec );
            if( i_channel != -1 )
                input_DecoderSetCcState( es->p_master->p_dec, false, i_channel );
        }
    }
    else
    {
        const int i_spu_id = var_GetInteger( p_input, "spu-es");
        int i;
        for( i = 0; i < 4; i++ )
        {
            if( !es->pb_cc_present[i] || !es->pp_cc_es[i] )
                continue;
            if( i_spu_id == es->pp_cc_es[i]->i_id )
            {
                /* Force unselection of the CC */
                input_SendEventEsSelect( p_input, SPU_ES, -1 );
            }
            EsOutDel( out, es->pp_cc_es[i] );
            es->pb_cc_present[i] = false;
        }
        EsDestroyDecoder( out, es );
    }
    if( !b_update )
        return;
    /* Mark it as unselected */
    input_SendEventEsSelect( p_input, es->fmt.i_cat, -1 );
    if( EsFmtIsTeletext( &es->fmt ) )
        input_SendEventTeletextSelect( p_input, -1 );
}
