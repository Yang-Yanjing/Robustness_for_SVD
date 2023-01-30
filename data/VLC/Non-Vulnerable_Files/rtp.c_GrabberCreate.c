}
static sout_access_out_t *GrabberCreate( sout_stream_t *p_stream )
{
    sout_access_out_t *p_grab;
    p_grab = vlc_object_create( p_stream, sizeof( *p_grab ) );
    if( p_grab == NULL )
        return NULL;
    p_grab->p_module    = NULL;
    p_grab->psz_access  = strdup( "grab" );
    p_grab->p_cfg       = NULL;
    p_grab->psz_path    = strdup( "" );
    p_grab->p_sys       = (sout_access_out_sys_t *)p_stream;
    p_grab->pf_seek     = NULL;
    p_grab->pf_write    = AccessOutGrabberWrite;
    return p_grab;
}
