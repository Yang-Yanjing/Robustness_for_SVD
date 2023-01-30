}
static int ControlLockedSetRate( es_out_t *p_out, int i_src_rate, int i_rate )
{
    es_out_sys_t *p_sys = p_out->p_sys;
    int i_ret;
    if( !p_sys->b_delayed && i_src_rate == i_rate )
    {
        i_ret = es_out_SetRate( p_sys->p_out, i_src_rate, i_rate );
    }
    else
    {
        i_ret = VLC_EGENERIC;
        if( !p_sys->p_input->p->b_can_pace_control )
        {
            if( !p_sys->b_delayed )
                TsStart( p_out );
            if( p_sys->b_delayed )
                i_ret = TsChangeRate( p_sys->p_ts, i_src_rate, i_rate );
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
        p_sys->i_input_rate_source = i_src_rate;
        p_sys->i_input_rate = i_rate;
    }
    return i_ret;
}
