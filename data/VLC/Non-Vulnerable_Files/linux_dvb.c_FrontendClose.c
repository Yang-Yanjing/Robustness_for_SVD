 *****************************************************************************/
void FrontendClose( access_t *p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    if( p_sys->p_frontend )
    {
        close( p_sys->i_frontend_handle );
        free( p_sys->p_frontend );
        p_sys->p_frontend = NULL;
    }
}
