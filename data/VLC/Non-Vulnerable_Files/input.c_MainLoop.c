 */
static void MainLoop( input_thread_t *p_input, bool b_interactive )
{
    mtime_t i_start_mdate = mdate();
    mtime_t i_intf_update = 0;
    mtime_t i_statistic_update = 0;
    mtime_t i_last_seek_mdate = 0;
    bool b_pause_after_eof = b_interactive &&
                             var_CreateGetBool( p_input, "play-and-pause" );
    while( vlc_object_alive( p_input ) && !p_input->b_error )
    {
        bool b_force_update;
        vlc_value_t val;
        mtime_t i_current;
        mtime_t i_wakeup;
        bool b_paused;
        bool b_demux_polled;
        /* Demux data */
        b_force_update = false;
        i_wakeup = 0;
        /* FIXME if p_input->p->i_state == PAUSE_S the access/access_demux
         * is paused -> this may cause problem with some of them
         * The same problem can be seen when seeking while paused */
        b_paused = p_input->p->i_state == PAUSE_S &&
                   ( !es_out_GetBuffering( p_input->p->p_es_out ) || p_input->p->input.b_eof );
        b_demux_polled = true;
        if( !b_paused )
        {
            if( !p_input->p->input.b_eof )
            {
                MainLoopDemux( p_input, &b_force_update, &b_demux_polled, i_start_mdate );
                i_wakeup = es_out_GetWakeup( p_input->p->p_es_out );
            }
            else if( !es_out_GetEmpty( p_input->p->p_es_out ) )
            {
                msg_Dbg( p_input, "waiting decoder fifos to empty" );
                i_wakeup = mdate() + INPUT_IDLE_SLEEP;
            }
            /* Pause after eof only if the input is pausable.
             * This way we won't trigger timeshifting for nothing */
            else if( b_pause_after_eof && p_input->p->b_can_pause )
            {
                msg_Dbg( p_input, "pausing at EOF (pause after each)");
                val.i_int = PAUSE_S;
                Control( p_input, INPUT_CONTROL_SET_STATE, val );
                b_paused = true;
            }
            else
            {
                if( MainLoopTryRepeat( p_input, &i_start_mdate ) )
                    break;
                b_pause_after_eof = var_GetBool( p_input, "play-and-pause" );
            }
        }
        /* */
        do {
            mtime_t i_deadline = i_wakeup;
            if( b_paused || !b_demux_polled )
                i_deadline = __MIN( i_intf_update, i_statistic_update );
            /* Handle control */
            for( ;; )
            {
                mtime_t i_limit = i_deadline;
                /* We will postpone the execution of a seek until we have
                 * finished the ES bufferisation (postpone is limited to
                 * 125ms) */
                bool b_buffering = es_out_GetBuffering( p_input->p->p_es_out ) &&
                                   !p_input->p->input.b_eof;
                if( b_buffering )
                {
                    /* When postpone is in order, check the ES level every 20ms */
                    mtime_t i_current = mdate();
                    if( i_last_seek_mdate + INT64_C(125000) >= i_current )
                        i_limit = __MIN( i_deadline, i_current + INT64_C(20000) );
                }
                int i_type;
                if( ControlPop( p_input, &i_type, &val, i_limit, b_buffering ) )
                {
                    if( b_buffering && i_limit < i_deadline )
                        continue;
                    break;
                }
#ifndef NDEBUG
                msg_Dbg( p_input, "control type=%d", i_type );
#endif
                if( Control( p_input, i_type, val ) )
                {
                    if( ControlIsSeekRequest( i_type ) )
                        i_last_seek_mdate = mdate();
                    b_force_update = true;
                }
            }
            /* Update interface and statistics */
            i_current = mdate();
            if( i_intf_update < i_current || b_force_update )
            {
                MainLoopInterface( p_input );
                i_intf_update = i_current + INT64_C(250000);
                b_force_update = false;
            }
            if( i_statistic_update < i_current )
            {
                MainLoopStatistic( p_input );
                i_statistic_update = i_current + INT64_C(1000000);
            }
            /* Update the wakeup time */
            if( i_wakeup != 0 )
                i_wakeup = es_out_GetWakeup( p_input->p->p_es_out );
        } while( i_current < i_wakeup );
    }
    if( !p_input->b_error )
        input_ChangeState( p_input, END_S );
}
