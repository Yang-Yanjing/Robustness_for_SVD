 *****************************************************************************/
static ssize_t Read( access_t *p_access, uint8_t *p_buffer, size_t i_len )
{
    access_sys_t *p_sys = p_access->p_sys;
    GnomeVFSFileSize i_read_len;
    int i_ret;
    i_ret = gnome_vfs_read( p_sys->p_handle, p_buffer,
                                  (GnomeVFSFileSize)i_len, &i_read_len );
    if( i_ret )
    {
        p_access->info.b_eof = true;
        if( i_ret != GNOME_VFS_ERROR_EOF )
        {
            msg_Err( p_access, "read failed (%s)",
                                    gnome_vfs_result_to_string( i_ret ) );
        }
    }
    p_access->info.i_pos += (int64_t)i_read_len;
    if( p_access->info.i_pos >= p_sys->p_file_info->size
     && p_sys->p_file_info->size != 0 && p_sys->b_local )
    {
        gnome_vfs_file_info_clear( p_sys->p_file_info );
        i_ret = gnome_vfs_get_file_info_from_handle( p_sys->p_handle,
                                                     p_sys->p_file_info, 8 );
        if( i_ret )
            msg_Warn( p_access, "couldn't get file properties again (%s)",
                      gnome_vfs_result_to_string( i_ret ) );
    }
    return (int)i_read_len;
}
