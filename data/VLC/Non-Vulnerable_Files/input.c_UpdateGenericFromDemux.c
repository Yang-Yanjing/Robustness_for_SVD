}
static void UpdateGenericFromDemux( input_thread_t *p_input )
{
    demux_t *p_demux = p_input->p->input.p_demux;
    if( p_demux->info.i_update & INPUT_UPDATE_META )
    {
        InputUpdateMeta( p_input, p_demux );
        p_demux->info.i_update &= ~INPUT_UPDATE_META;
    }
    {
        double quality;
        double strength;
        if( !demux_Control( p_demux, DEMUX_GET_SIGNAL, &quality, &strength ) )
            input_SendEventSignal( p_input, quality, strength );
    }
}
