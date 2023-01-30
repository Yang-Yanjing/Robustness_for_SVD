 *****************************************************************************/
static int UpdateTitleSeekpointFromDemux( input_thread_t *p_input )
{
    demux_t *p_demux = p_input->p->input.p_demux;
    /* TODO event-like */
    if( p_demux->info.i_update & INPUT_UPDATE_TITLE )
    {
        input_SendEventTitle( p_input, p_demux->info.i_title );
        p_demux->info.i_update &= ~INPUT_UPDATE_TITLE;
    }
    if( p_demux->info.i_update & INPUT_UPDATE_SEEKPOINT )
    {
        input_SendEventSeekpoint( p_input,
                                  p_demux->info.i_title, p_demux->info.i_seekpoint );
        p_demux->info.i_update &= ~INPUT_UPDATE_SEEKPOINT;
    }
    /* Hmmm only works with master input */
    if( p_input->p->input.p_demux == p_demux )
        return UpdateTitleSeekpoint( p_input,
                                     p_demux->info.i_title,
                                     p_demux->info.i_seekpoint );
    return 1;
}
