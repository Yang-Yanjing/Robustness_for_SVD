}
static void *TsRun( void *p_data )
{
    ts_thread_t *p_ts = p_data;
    mtime_t i_buffering_date = -1;
    for( ;; )
    {
        ts_cmd_t cmd;
        mtime_t  i_deadline;
        bool b_buffering;
        /* Pop a command to execute */
        vlc_mutex_lock( &p_ts->lock );
        mutex_cleanup_push( &p_ts->lock );
        for( ;; )
        {
            const int canc = vlc_savecancel();
            b_buffering = es_out_GetBuffering( p_ts->p_out );
            if( ( !p_ts->b_paused || b_buffering ) && !TsPopCmdLocked( p_ts, &cmd, false ) )
            {
                vlc_restorecancel( canc );
                break;
            }
            vlc_restorecancel( canc );
            vlc_cond_wait( &p_ts->wait, &p_ts->lock );
        }
        if( b_buffering && i_buffering_date < 0 )
        {
            i_buffering_date = cmd.i_date;
        }
        else if( i_buffering_date > 0 )
        {
            p_ts->i_buffering_delay += i_buffering_date - cmd.i_date; /* It is < 0 */
            if( b_buffering )
                i_buffering_date = cmd.i_date;
            else
                i_buffering_date = -1;
        }
        if( p_ts->i_rate_date < 0 )
            p_ts->i_rate_date = cmd.i_date;
        p_ts->i_rate_delay = 0;
        if( p_ts->i_rate_source != p_ts->i_rate )
        {
            const mtime_t i_duration = cmd.i_date - p_ts->i_rate_date;
            p_ts->i_rate_delay = i_duration * p_ts->i_rate / p_ts->i_rate_source - i_duration;
        }
        if( p_ts->i_cmd_delay + p_ts->i_rate_delay + p_ts->i_buffering_delay < 0 && p_ts->i_rate != p_ts->i_rate_source )
        {
            const int canc = vlc_savecancel();
            /* Auto reset to rate 1.0 */
            msg_Warn( p_ts->p_input, "es out timeshift: auto reset rate to %d", p_ts->i_rate_source );
            p_ts->i_cmd_delay = 0;
            p_ts->i_buffering_delay = 0;
            p_ts->i_rate_delay = 0;
            p_ts->i_rate_date = -1;
            p_ts->i_rate = p_ts->i_rate_source;
            if( !es_out_SetRate( p_ts->p_out, p_ts->i_rate_source, p_ts->i_rate ) )
            {
                vlc_value_t val = { .i_int = p_ts->i_rate };
                /* Warn back input
                 * FIXME it is perfectly safe BUT it is ugly as it may hide a
                 * rate change requested by user */
                input_ControlPush( p_ts->p_input, INPUT_CONTROL_SET_RATE, &val );
            }
            vlc_restorecancel( canc );
        }
        i_deadline = cmd.i_date + p_ts->i_cmd_delay + p_ts->i_rate_delay + p_ts->i_buffering_delay;
        vlc_cleanup_run();
        /* Regulate the speed of command processing to the same one than
         * reading  */
        vlc_cleanup_push( cmd_cleanup_routine, &cmd );
        mwait( i_deadline );
        vlc_cleanup_pop();
        /* Execute the command  */
        const int canc = vlc_savecancel();
        switch( cmd.i_type )
        {
        case C_ADD:
            CmdExecuteAdd( p_ts->p_out, &cmd );
            CmdCleanAdd( &cmd );
            break;
        case C_SEND:
            CmdExecuteSend( p_ts->p_out, &cmd );
            CmdCleanSend( &cmd );
            break;
        case C_CONTROL:
            CmdExecuteControl( p_ts->p_out, &cmd );
            CmdCleanControl( &cmd );
            break;
        case C_DEL:
            CmdExecuteDel( p_ts->p_out, &cmd );
            break;
        default:
            assert(0);
            break;
        }
        vlc_restorecancel( canc );
    }
    return NULL;
}
