}
static int ScanDvbSNext( scan_t *p_scan, scan_configuration_t *p_cfg, double *pf_pos )
{
    if( p_scan->parameter.b_exhaustive )
        msg_Dbg( p_scan->p_obj, "no exhaustive svb-d scan mode" );
    return ScanDvbSNextFast( p_scan, p_cfg, pf_pos );
}
