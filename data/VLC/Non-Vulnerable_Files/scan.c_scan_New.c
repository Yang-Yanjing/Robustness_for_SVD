/* */
scan_t *scan_New( vlc_object_t *p_obj, const scan_parameter_t *p_parameter )
{
    if( p_parameter->type == SCAN_DVB_T )
    {
        msg_Dbg( p_obj, "DVB-T scanning:" );
        msg_Dbg( p_obj, " - frequency [%d, %d]",
                 p_parameter->frequency.i_min, p_parameter->frequency.i_max );
        msg_Dbg( p_obj, " - bandwidth [%d,%d]",
                 p_parameter->bandwidth.i_min, p_parameter->bandwidth.i_max );
        msg_Dbg( p_obj, " - exhaustive mode %s", p_parameter->b_exhaustive ? "on" : "off" );
    }
    else if( p_parameter->type == SCAN_DVB_C )
    {
        msg_Dbg( p_obj, "DVB-C scanning:" );
        msg_Dbg( p_obj, " - frequency [%d, %d]",
                 p_parameter->frequency.i_min, p_parameter->frequency.i_max );
        msg_Dbg( p_obj, " - bandwidth [%d,%d]",
                 p_parameter->bandwidth.i_min, p_parameter->bandwidth.i_max );
        msg_Dbg( p_obj, " - exhaustive mode %s", p_parameter->b_exhaustive ? "on" : "off" );
        msg_Dbg( p_obj, " - scannin modulations %s", p_parameter->b_modulation_set ? "off" : "on" );
    }
    else if( p_parameter->type == SCAN_DVB_S )
    {
        msg_Dbg( p_obj, "DVB-S scanning:" );
        msg_Dbg( p_obj, " - satellite [%s]", p_parameter->sat_info.psz_name );
    }
    else
    {
        return NULL;
    }
    msg_Dbg( p_obj, " - use NIT %s", p_parameter->b_use_nit ? "on" : "off" );
    msg_Dbg( p_obj, " - FTA only %s", p_parameter->b_free_only ? "on" : "off" );
    scan_t *p_scan = malloc( sizeof( *p_scan ) );
    if( unlikely(p_scan == NULL) )
        return NULL;
    p_scan->p_obj = VLC_OBJECT(p_obj);
    p_scan->i_index = 0;
    p_scan->p_dialog = NULL;
    TAB_INIT( p_scan->i_service, p_scan->pp_service );
    p_scan->parameter = *p_parameter;
    p_scan->i_time_start = mdate();
    return p_scan;
}
