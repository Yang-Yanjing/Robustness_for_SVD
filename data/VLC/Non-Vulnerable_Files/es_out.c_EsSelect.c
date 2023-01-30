}
static void EsSelect( es_out_t *out, es_out_id_t *es )
{
    es_out_sys_t   *p_sys = out->p_sys;
    input_thread_t *p_input = p_sys->p_input;
    if( EsIsSelected( es ) )
    {
        msg_Warn( p_input, "ES 0x%x is already selected", es->i_id );
        return;
    }
    if( es->p_master )
    {
        int i_channel;
        if( !es->p_master->p_dec )
            return;
        i_channel = EsOutGetClosedCaptionsChannel( es->fmt.i_codec );
        if( i_channel == -1 || input_DecoderSetCcState( es->p_master->p_dec, true, i_channel ) )
            return;
    }
    else
    {
        const bool b_sout = p_input->p->p_sout != NULL;
        if( es->fmt.i_cat == VIDEO_ES || es->fmt.i_cat == SPU_ES )
        {
            if( !var_GetBool( p_input, b_sout ? "sout-video" : "video" ) )
            {
                msg_Dbg( p_input, "video is disabled, not selecting ES 0x%x",
                         es->i_id );
                return;
            }
        }
        else if( es->fmt.i_cat == AUDIO_ES )
        {
            if( !var_GetBool( p_input, b_sout ? "sout-audio" : "audio" ) )
            {
                msg_Dbg( p_input, "audio is disabled, not selecting ES 0x%x",
                         es->i_id );
                return;
            }
        }
        if( es->fmt.i_cat == SPU_ES )
        {
            if( !var_GetBool( p_input, b_sout ? "sout-spu" : "spu" ) )
            {
                msg_Dbg( p_input, "spu is disabled, not selecting ES 0x%x",
                         es->i_id );
                return;
            }
        }
        EsCreateDecoder( out, es );
        if( es->p_dec == NULL || es->p_pgrm != p_sys->p_pgrm )
            return;
    }
    /* Mark it as selected */
    input_SendEventEsSelect( p_input, es->fmt.i_cat, es->i_id );
    input_SendEventTeletextSelect( p_input, EsFmtIsTeletext( &es->fmt ) ? es->i_id : -1 );
}
