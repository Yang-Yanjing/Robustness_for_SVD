}
static int ScanDvbTNext( scan_t *p_scan, scan_configuration_t *p_cfg, double *pf_pos )
{
    if( p_scan->parameter.b_exhaustive )
        return ScanDvbNextExhaustive( p_scan, p_cfg, pf_pos );
    else
        return ScanDvbTNextFast( p_scan, p_cfg, pf_pos );
}
