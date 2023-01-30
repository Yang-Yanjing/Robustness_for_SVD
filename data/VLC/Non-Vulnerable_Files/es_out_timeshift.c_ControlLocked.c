}
static int ControlLocked( es_out_t *p_out, int i_query, va_list args )
{
    es_out_sys_t *p_sys = p_out->p_sys;
    switch( i_query )
    {
    /* Invalid query for this es_out level */
    case ES_OUT_SET_ES_BY_ID:
    case ES_OUT_RESTART_ES_BY_ID:
    case ES_OUT_SET_ES_DEFAULT_BY_ID:
    case ES_OUT_GET_ES_OBJECTS_BY_ID:
    case ES_OUT_SET_DELAY:
    case ES_OUT_SET_RECORD_STATE:
        assert(0);
        return VLC_EGENERIC;
    /* Pass-through control */
    case ES_OUT_SET_MODE:
    case ES_OUT_SET_GROUP:
    case ES_OUT_SET_PCR:
    case ES_OUT_SET_GROUP_PCR:
    case ES_OUT_RESET_PCR:
    case ES_OUT_SET_NEXT_DISPLAY_TIME:
    case ES_OUT_SET_GROUP_META:
    case ES_OUT_SET_GROUP_EPG:
    case ES_OUT_SET_ES_SCRAMBLED_STATE:
    case ES_OUT_DEL_GROUP:
    case ES_OUT_SET_META:
    case ES_OUT_SET_ES:
    case ES_OUT_RESTART_ES:
    case ES_OUT_SET_ES_DEFAULT:
    case ES_OUT_SET_ES_STATE:
    case ES_OUT_SET_ES_FMT:
    case ES_OUT_SET_TIMES:
    case ES_OUT_SET_JITTER:
    case ES_OUT_SET_EOS:
    {
        ts_cmd_t cmd;
        if( CmdInitControl( &cmd, i_query, args, p_sys->b_delayed ) )
            return VLC_EGENERIC;
        if( p_sys->b_delayed )
        {
            TsPushCmd( p_sys->p_ts, &cmd );
            return VLC_SUCCESS;
        }
        return CmdExecuteControl( p_sys->p_out, &cmd );
    }
    /* Special control when delayed */
    case ES_OUT_GET_ES_STATE:
    {
        es_out_id_t *p_es = (es_out_id_t*)va_arg( args, es_out_id_t * );
        bool *pb_enabled = (bool*)va_arg( args, bool* );
        if( p_sys->b_delayed )
        {
            *pb_enabled = true;
            return VLC_SUCCESS;
        }
        return es_out_Control( p_sys->p_out, ES_OUT_GET_ES_STATE, p_es->p_es, pb_enabled );
    }
    /* Special internal input control */
    case ES_OUT_GET_EMPTY:
    {
        bool *pb_empty = (bool*)va_arg( args, bool* );
        return ControlLockedGetEmpty( p_out, pb_empty );
    }
    case ES_OUT_GET_WAKE_UP: /* TODO ? */
    {
        mtime_t *pi_wakeup = (mtime_t*)va_arg( args, mtime_t* );
        return ControlLockedGetWakeup( p_out, pi_wakeup );
    }
    case ES_OUT_GET_BUFFERING:
    {
        bool *pb_buffering = (bool *)va_arg( args, bool* );
        return ControlLockedGetBuffering( p_out, pb_buffering );
    }
    case ES_OUT_SET_PAUSE_STATE:
    {
        const bool b_source_paused = (bool)va_arg( args, int );
        const bool b_paused = (bool)va_arg( args, int );
        const mtime_t i_date = (mtime_t) va_arg( args, mtime_t );
        return ControlLockedSetPauseState( p_out, b_source_paused, b_paused, i_date );
    }
    case ES_OUT_SET_RATE:
    {
        const int i_src_rate = (int)va_arg( args, int );
        const int i_rate = (int)va_arg( args, int );
        return ControlLockedSetRate( p_out, i_src_rate, i_rate );
    }
    case ES_OUT_SET_TIME:
    {
        const mtime_t i_date = (mtime_t)va_arg( args, mtime_t );
        return ControlLockedSetTime( p_out, i_date );
    }
    case ES_OUT_SET_FRAME_NEXT:
    {
        return ControlLockedSetFrameNext( p_out );
    }
    case ES_OUT_GET_PCR_SYSTEM:
    {
        if( p_sys->b_delayed )
            return VLC_EGENERIC;
        mtime_t *pi_system = (mtime_t*)va_arg( args, mtime_t * );
        mtime_t *pi_delay  = (mtime_t*)va_arg( args, mtime_t * );
        return es_out_ControlGetPcrSystem( p_sys->p_out, pi_system, pi_delay );
    }
    case ES_OUT_MODIFY_PCR_SYSTEM:
    {
        const bool    b_absolute = va_arg( args, int );
        const mtime_t i_system   = va_arg( args, mtime_t );
        if( b_absolute && p_sys->b_delayed )
            return VLC_EGENERIC;
        return es_out_ControlModifyPcrSystem( p_sys->p_out, b_absolute, i_system );
    }
    case ES_OUT_GET_GROUP_FORCED:
    {
        int *pi_group = va_arg( args, int * );
        return es_out_Control( p_sys->p_out, ES_OUT_GET_GROUP_FORCED, pi_group );
    }
    default:
        msg_Err( p_sys->p_input, "Unknown es_out_Control query !" );
        assert(0);
        return VLC_EGENERIC;
    }
}
