 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    access_t     *p_access = (access_t *)p_this;
    access_sys_t *p_sys = p_access->p_sys;
    free( p_sys->p_sectors );
    ioctl_Close( p_this, p_sys->vcddev );
    free( p_sys );
}
