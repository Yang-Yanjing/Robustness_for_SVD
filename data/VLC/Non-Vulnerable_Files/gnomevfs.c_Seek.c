 *****************************************************************************/
static int Seek( access_t *p_access, uint64_t i_pos )
{
    access_sys_t *p_sys = p_access->p_sys;
    int i_ret;
    i_ret = gnome_vfs_seek( p_sys->p_handle, GNOME_VFS_SEEK_START,
                                            (GnomeVFSFileOffset)i_pos);
    if ( !i_ret )
    {
        p_access->info.i_pos = i_pos;
    }
    else
    {
        GnomeVFSFileSize i_offset;
        msg_Err( p_access, "cannot seek (%s)",
                                        gnome_vfs_result_to_string( i_ret ) );
        i_ret = gnome_vfs_tell( p_sys->p_handle, &i_offset );
        if( !i_ret )
        {
            msg_Err( p_access, "cannot tell the current position (%s)",
                                        gnome_vfs_result_to_string( i_ret ) );
            return VLC_EGENERIC;
        }
    }
    /* Reset eof */
    p_access->info.b_eof = false;
    /* FIXME */
    return VLC_SUCCESS;
}
