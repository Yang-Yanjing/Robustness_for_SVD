}
static scan_service_t *ScanFindService( scan_t *p_scan, int i_service_start, int i_program )
{
    for( int i = i_service_start; i < p_scan->i_service; i++ )
    {
        if( p_scan->pp_service[i]->i_program == i_program )
            return p_scan->pp_service[i];
    }
    return NULL;
}
