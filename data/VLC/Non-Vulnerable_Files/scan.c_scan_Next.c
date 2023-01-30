}
int scan_Next( scan_t *p_scan, scan_configuration_t *p_cfg )
{
    double f_position;
    int i_ret;
    if( scan_IsCancelled( p_scan ) )
        return VLC_EGENERIC;
    memset( p_cfg, 0, sizeof(*p_cfg) );
    switch( p_scan->parameter.type )
    {
    case SCAN_DVB_T:
        i_ret = ScanDvbTNext( p_scan, p_cfg, &f_position );
        break;
    case SCAN_DVB_C:
        i_ret = ScanDvbCNext( p_scan, p_cfg, &f_position );
        break;
    case SCAN_DVB_S:
        i_ret = ScanDvbSNext( p_scan, p_cfg, &f_position );
        break;
    default:
        i_ret = VLC_EGENERIC;
        break;
    }
    if( i_ret )
        return i_ret;
    char *psz_text;
    int i_service = 0;
    for( int i = 0; i < p_scan->i_service; i++ )
    {
        if( p_scan->pp_service[i]->psz_name )
            i_service++;
    }
    const mtime_t i_eta = f_position > 0.005 ? (mdate() - p_scan->i_time_start) * ( 1.0 / f_position - 1.0 ) : -1;
    char psz_eta[MSTRTIME_MAX_SIZE];
    if( asprintf( &psz_text, _("%.1f MHz (%d services)\n~%s remaining"),
                  (double)p_cfg->i_frequency / 1000000, i_service, secstotimestr( psz_eta, i_eta/1000000 ) ) >= 0 )
    {
        if( i_eta >= 0 )
            msg_Info( p_scan->p_obj, "Scan ETA %s | %f", secstotimestr( psz_eta, i_eta/1000000 ), f_position * 100 );
        if( p_scan->p_dialog == NULL )
            p_scan->p_dialog = dialog_ProgressCreate( p_scan->p_obj, _("Scanning DVB"), psz_text, _("Cancel") );
        if( p_scan->p_dialog != NULL )
            dialog_ProgressSet( p_scan->p_dialog, psz_text, f_position );
        free( psz_text );
    }
    p_scan->i_index++;
    return VLC_SUCCESS;
}
