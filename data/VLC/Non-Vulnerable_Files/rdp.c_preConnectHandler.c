/* instance handlers */
static bool preConnectHandler( freerdp *p_instance )
{
    vlcrdp_context_t * p_vlccontext = (vlcrdp_context_t *) p_instance->context;
    demux_sys_t *p_sys = p_vlccontext->p_demux->p_sys;
    /* Configure connexion */
    p_instance->settings->SoftwareGdi = true; /* render in buffer */
    p_instance->settings->Fullscreen = true;
    p_instance->settings->ServerHostname = strdup( p_sys->psz_hostname );
    p_instance->settings->Username =
            var_InheritString( p_vlccontext->p_demux, CFG_PREFIX "user" );
    p_instance->settings->Password =
            var_InheritString( p_vlccontext->p_demux, CFG_PREFIX "password" );
    p_instance->settings->ServerPort = p_sys->i_port;
    p_instance->settings->EncryptionMethods =
            var_InheritBool( p_vlccontext->p_demux, CFG_PREFIX "encrypt" );
    return true;
}
