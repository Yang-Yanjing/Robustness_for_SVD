}
static int ScanParametersDvbS( access_t *p_access, scan_parameter_t *p_scan )
{
    const frontend_t *p_frontend = p_access->p_sys->p_frontend;
    memset( p_scan, 0, sizeof(*p_scan) );
    p_scan->type = SCAN_DVB_S;
    p_scan->frequency.i_min = p_frontend->info.frequency_min;
    p_scan->frequency.i_max = p_frontend->info.frequency_max;
    /* set satellite config file path */
    p_scan->sat_info.psz_name = var_InheritString( p_access, "dvb-satellite" );
    return VLC_SUCCESS;
}
