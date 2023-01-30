}
static int ScanDvbNextExhaustive( scan_t *p_scan, scan_configuration_t *p_cfg, double *pf_pos )
{
    if( p_scan->i_index > p_scan->parameter.frequency.i_count * p_scan->parameter.bandwidth.i_count )
        return VLC_EGENERIC;
    const int i_bi = p_scan->i_index % p_scan->parameter.bandwidth.i_count;
    const int i_fi = p_scan->i_index / p_scan->parameter.bandwidth.i_count;
    p_cfg->i_frequency = p_scan->parameter.frequency.i_min + i_fi * p_scan->parameter.frequency.i_step;
    p_cfg->i_bandwidth = p_scan->parameter.bandwidth.i_min + i_bi * p_scan->parameter.bandwidth.i_step;
    *pf_pos = (double)p_scan->i_index / p_scan->parameter.frequency.i_count;
    return VLC_SUCCESS;
}
