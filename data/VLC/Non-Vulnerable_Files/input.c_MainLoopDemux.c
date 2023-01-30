 */
static void MainLoopDemux( input_thread_t *p_input, bool *pb_changed, bool *pb_demux_polled, mtime_t i_start_mdate )
{
    int i_ret;
    *pb_changed = false;
    *pb_demux_polled = p_input->p->input.p_demux->pf_demux != NULL;
    if( ( p_input->p->i_stop > 0 && p_input->p->i_time >= p_input->p->i_stop ) ||
        ( p_input->p->i_run > 0 && i_start_mdate+p_input->p->i_run < mdate() ) )
        i_ret = 0; /* EOF */
    else
        i_ret = demux_Demux( p_input->p->input.p_demux );
    if( i_ret > 0 )
    {
        if( p_input->p->input.p_demux->info.i_update )
        {
            if( p_input->p->input.p_demux->info.i_update & INPUT_UPDATE_TITLE_LIST )
            {
                UpdateTitleListfromDemux( p_input );
                p_input->p->input.p_demux->info.i_update &= ~INPUT_UPDATE_TITLE_LIST;
            }
            if( p_input->p->input.b_title_demux )
            {
                i_ret = UpdateTitleSeekpointFromDemux( p_input );
                *pb_changed = true;
            }
            UpdateGenericFromDemux( p_input );
        }
    }
    if( i_ret == 0 )    /* EOF */
    {
        msg_Dbg( p_input, "EOF reached" );
        p_input->p->input.b_eof = true;
        es_out_Eos(p_input->p->p_es_out);
    }
    else if( i_ret < 0 )
    {
        input_ChangeState( p_input, ERROR_S );
    }
    if( i_ret > 0 && p_input->p->i_slave > 0 )
    {
        bool b_demux_polled;
        SlaveDemux( p_input, &b_demux_polled );
        *pb_demux_polled |= b_demux_polled;
    }
}
