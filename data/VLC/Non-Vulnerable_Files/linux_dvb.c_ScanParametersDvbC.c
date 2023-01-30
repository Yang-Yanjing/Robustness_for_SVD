}
static int ScanParametersDvbC( access_t *p_access, scan_parameter_t *p_scan )
{
    const frontend_t *p_frontend = p_access->p_sys->p_frontend;
    memset( p_scan, 0, sizeof(*p_scan) );
    p_scan->type = SCAN_DVB_C;
    p_scan->b_exhaustive = false;
    /* */
    p_scan->frequency.i_min = p_frontend->info.frequency_min;
    p_scan->frequency.i_max = p_frontend->info.frequency_max;
    p_scan->frequency.i_step = p_frontend->info.frequency_stepsize
        ? p_frontend->info.frequency_stepsize : 166667;
    p_scan->frequency.i_count = (p_scan->frequency.i_max-p_scan->frequency.i_min)/p_scan->frequency.i_step;
    /* if frontend can do auto, don't scan them */
    if( p_frontend->info.caps & FE_CAN_QAM_AUTO )
    {
        p_scan->b_modulation_set = true;
    } else {
        p_scan->b_modulation_set = false;
        /* our scanning code flips modulation from 16..256 automaticly*/
        p_scan->i_modulation = 0;
    }
    /* if user supplies symbolrate, don't scan those */
    if( var_GetInteger( p_access, "dvb-srate" ) )
        p_scan->b_symbolrate_set = true;
    else
        p_scan->b_symbolrate_set = false;
    /* */
    p_scan->bandwidth.i_min  = 6;
    p_scan->bandwidth.i_max  = 8;
    p_scan->bandwidth.i_step = 1;
    p_scan->bandwidth.i_count = 3;
    return VLC_SUCCESS;
}
