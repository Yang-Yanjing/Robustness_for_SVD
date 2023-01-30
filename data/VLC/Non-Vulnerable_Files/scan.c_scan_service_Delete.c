}
static void scan_service_Delete( scan_service_t *p_srv )
{
    free( p_srv->psz_name );
    free( p_srv );
}
