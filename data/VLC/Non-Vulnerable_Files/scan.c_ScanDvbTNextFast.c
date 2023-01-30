}
static int ScanDvbTNextFast( scan_t *p_scan, scan_configuration_t *p_cfg, double *pf_pos )
{
    static const int i_band_count = 2;
    static const struct
    {
        const char *psz_name;
        int i_min;
        int i_max;
    }
    band[2] =
    {
        { "VHF", 174, 230 },
        { "UHF", 470, 862 },
    };
    const int i_offset_count = 5;
    const int i_mhz = 1000000;
    /* We will probe the whole band divided in all bandwidth possibility trying 
     * i_offset_count offset around the position
     */
    for( ;; p_scan->i_index++ )
    {
        const int i_bi = p_scan->i_index % p_scan->parameter.bandwidth.i_count;
        const int i_oi = (p_scan->i_index / p_scan->parameter.bandwidth.i_count) % i_offset_count;
        const int i_fi = (p_scan->i_index / p_scan->parameter.bandwidth.i_count) / i_offset_count;
        const int i_bandwidth = p_scan->parameter.bandwidth.i_min + i_bi * p_scan->parameter.bandwidth.i_step;
        int i;
        for( i = 0; i < i_band_count; i++ )
        {
            if( i_fi >= band[i].i_min && i_fi <= band[i].i_max )
                break;
        }
        if( i >=i_band_count )
        {
            if( i_fi > band[i_band_count-1].i_max )
                return VLC_EGENERIC;
            continue;
        }
        const int i_frequency_min = band[i].i_min*i_mhz + i_bandwidth*i_mhz/2;
        const int i_frequency_base = i_fi*i_mhz;
        if( i_frequency_base >= i_frequency_min && ( i_frequency_base - i_frequency_min ) % ( i_bandwidth*i_mhz ) == 0 )
        {
            const int i_frequency = i_frequency_base + ( i_oi - i_offset_count/2 ) * p_scan->parameter.frequency.i_step;
            if( i_frequency < p_scan->parameter.frequency.i_min ||
                i_frequency > p_scan->parameter.frequency.i_max )
                continue;
            p_cfg->i_frequency = i_frequency;
            p_cfg->i_bandwidth = i_bandwidth;
            int i_current = 0, i_total = 0;
            for( int i = 0; i < i_band_count; i++ )
            {
                const int i_frag = band[i].i_max-band[i].i_min;
                if( i_fi >= band[i].i_min )
                    i_current += __MIN( i_fi - band[i].i_min, i_frag );
                i_total += i_frag;
            }
            *pf_pos = (double)( i_current + (double)i_oi / i_offset_count ) / i_total;
            return VLC_SUCCESS;
        }
    }
}
