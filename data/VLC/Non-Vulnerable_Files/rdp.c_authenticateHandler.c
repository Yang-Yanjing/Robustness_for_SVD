}
static bool authenticateHandler( freerdp *p_instance, char** ppsz_username,
                                 char** ppsz_password, char** ppsz_domain )
{
    VLC_UNUSED(ppsz_domain);
    vlcrdp_context_t * p_vlccontext = (vlcrdp_context_t *) p_instance->context;
    *ppsz_username = var_InheritString( p_vlccontext->p_demux, CFG_PREFIX "user" );
    *ppsz_password = var_InheritString( p_vlccontext->p_demux, CFG_PREFIX "password" );
    return true;
}
