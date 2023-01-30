/* */
static scan_service_t *scan_service_New( int i_program,
                                         const scan_configuration_t *p_cfg )
{
    scan_service_t *p_srv = malloc( sizeof(*p_srv) );
    if( !p_srv )
        return NULL;
    p_srv->i_program = i_program;
    p_srv->cfg = *p_cfg;
    p_srv->i_snr = -1;
    p_srv->type = SERVICE_UNKNOWN;
    p_srv->psz_name = NULL;
    p_srv->i_channel = -1;
    p_srv->b_crypted = false;
    p_srv->i_network_id = -1;
    p_srv->i_nit_version = -1;
    p_srv->i_sdt_version = -1;
    return p_srv;
}
