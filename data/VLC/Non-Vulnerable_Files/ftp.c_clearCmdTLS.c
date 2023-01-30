}
static void clearCmdTLS( access_sys_t *p_sys )
{
    if ( p_sys->cmd.p_tls ) vlc_tls_SessionDelete( p_sys->cmd.p_tls );
    if ( p_sys->p_creds ) vlc_tls_Delete( p_sys->p_creds );
    p_sys->cmd.p_tls = NULL;
    p_sys->cmd.p_vs = NULL;
    p_sys->p_creds = NULL;
}
