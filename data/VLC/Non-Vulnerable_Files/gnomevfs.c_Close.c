 *****************************************************************************/
static void Close( vlc_object_t * p_this )
{
    access_t     *p_access = (access_t*)p_this;
    access_sys_t *p_sys = p_access->p_sys;
    int i_result;
    i_result = gnome_vfs_close( p_sys->p_handle );
    if( i_result )
    {
         msg_Err( p_access, "cannot close %s: %s", p_sys->psz_name,
                                gnome_vfs_result_to_string( i_result ) );
    }
    gnome_vfs_file_info_unref( p_sys->p_file_info );
    free( p_sys->psz_name );
    free( p_sys );
}
