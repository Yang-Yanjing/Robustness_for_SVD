}
void scan_Destroy( scan_t *p_scan )
{
    if( p_scan->p_dialog != NULL )
        dialog_ProgressDestroy( p_scan->p_dialog );
    for( int i = 0; i < p_scan->i_service; i++ )
        scan_service_Delete( p_scan->pp_service[i] );
    TAB_CLEAN( p_scan->i_service, p_scan->pp_service );
    free( p_scan );
}
