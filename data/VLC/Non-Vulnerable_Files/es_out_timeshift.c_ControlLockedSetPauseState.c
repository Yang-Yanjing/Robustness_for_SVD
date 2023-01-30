}
static int ControlLockedSetPauseState( es_out_t *p_out, bool b_source_paused, bool b_paused, mtime_t i_date )
{
    es_out_sys_t *p_sys = p_out->p_sys;
    int i_ret;
    if( !p_sys->b_delayed && !b_source_paused == !b_paused )
    {
        i_ret = es_out_SetPauseState( p_sys->p_out, b_source_paused, b_paused, i_date );
    }
    else
    {
        i_ret = VLC_EGENERIC;
        if( !p_sys->p_input->p->b_can_pace_control )
        {
            if( !p_sys->b_delayed )
                TsStart( p_out );
            if( p_sys->b_delayed )
                i_ret = TsChangePause( p_sys->p_ts, b_source_paused, b_paused, i_date );
        }
        else
        {
            /* XXX we may do it BUT it would be better to finish the clock clean up+improvments
             * and so be able to advertize correctly pace control property in access
             * module */
            msg_Err( p_sys->p_input, "EsOutTimeshift does not work with streams that have pace control" );
        }
    }
    if( !i_ret )
    {
        p_sys->b_input_paused_source = b_source_paused;
        p_sys->b_input_paused = b_paused;
    }
    return i_ret;
}
