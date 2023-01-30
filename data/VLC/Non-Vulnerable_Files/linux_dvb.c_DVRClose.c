 *****************************************************************************/
void DVRClose( access_t * p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    close( p_sys->i_handle );
}
