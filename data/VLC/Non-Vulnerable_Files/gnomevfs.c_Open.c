 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    access_t       *p_access = (access_t*)p_this;
    access_sys_t   *p_sys = NULL;
    char           *psz_name = NULL;
    char           *psz_uri = NULL;
    char           *psz_unescaped = NULL;
    char           *psz_expand_tilde = NULL;
    GnomeVFSURI    *p_uri = NULL;
    GnomeVFSResult  i_ret;
    GnomeVFSHandle *p_handle = NULL;
    if( !(gnome_vfs_init()) )
    {
        msg_Warn( p_access, "couldn't initilize GnomeVFS" );
        return VLC_EGENERIC;
    }
    /* FIXME
       Since GnomeVFS segfaults on exit if we initialize it without trying to
       open a file with a valid protocol, try to open at least file:// */
    gnome_vfs_open( &p_handle, "file://", 5 );
    STANDARD_READ_ACCESS_INIT;
    p_sys->p_handle = p_handle;
    p_sys->b_pace_control = true;
    if( strcmp( "gnomevfs", p_access->psz_access ) &&
                                            *(p_access->psz_access) != '\0')
    {
        asprintf( &psz_name, "%s://%s", p_access->psz_access,
                  p_access->psz_location );
    }
    else
    {
        psz_name = strdup( p_access->psz_location );
    }
    psz_expand_tilde = gnome_vfs_expand_initial_tilde( psz_name );
    psz_unescaped = gnome_vfs_make_uri_from_shell_arg( psz_expand_tilde );
   /* gnome_vfs_make_uri_from_shell_arg will only escape the uri
      for relative paths. So we need to use
      gnome_vfs_escape_host_and_path_string in other cases. */
    if( !strcmp( psz_unescaped, psz_expand_tilde ) )
    {
    /* Now we are sure that we have a complete valid unescaped URI beginning
       with the protocol. We want to escape it. However, gnomevfs's escaping
       function are broken and will try to escape characters un the username/
       password field. So parse the URI with vlc_UrlParse ans only escape the
       path */
        vlc_url_t url;
        char *psz_escaped_path;
        char *psz_path_begin;
        vlc_UrlParse( &url, psz_unescaped, 0 );
        psz_escaped_path = gnome_vfs_escape_path_string( url.psz_path );
        if( psz_escaped_path )
        {
    /* Now let's reconstruct a valid URI from all that stuff */
            psz_path_begin = psz_unescaped + strlen( psz_unescaped )
                                           - strlen( url.psz_path );
            *psz_path_begin = '\0';
            asprintf( &psz_uri, "%s%s", psz_unescaped, psz_escaped_path );
            g_free( psz_escaped_path );
            g_free( psz_unescaped );
        }
        else
        {
            psz_uri = psz_unescaped;
        }
    }
    else
    {
        psz_uri = psz_unescaped;
    }
    g_free( psz_expand_tilde );
    p_uri = gnome_vfs_uri_new( psz_uri );
    if( p_uri )
    {
        p_sys->p_file_info = gnome_vfs_file_info_new();
        i_ret = gnome_vfs_get_file_info_uri( p_uri,
                                                p_sys->p_file_info, 8 );
        if( i_ret )
        {
            msg_Warn( p_access, "cannot get file info for uri %s (%s)",
                                psz_uri, gnome_vfs_result_to_string( i_ret ) );
            gnome_vfs_file_info_unref( p_sys->p_file_info );
            gnome_vfs_uri_unref( p_uri);
            free( p_sys );
            free( psz_uri );
            free( psz_name );
            return VLC_EGENERIC;
        }
    }
    else
    {
        msg_Warn( p_access, "cannot parse MRL %s or unsupported protocol", psz_name );
        free( psz_uri );
        free( p_sys );
        free( psz_name );
        return VLC_EGENERIC;
    }
    msg_Dbg( p_access, "opening file `%s'", psz_uri );
    i_ret = gnome_vfs_open( &(p_sys->p_handle), psz_uri, 5 );
    if( i_ret )
    {
        msg_Warn( p_access, "cannot open file %s: %s", psz_uri,
                                gnome_vfs_result_to_string( i_ret ) );
        gnome_vfs_uri_unref( p_uri);
        free( psz_uri );
        free( p_sys );
        free( psz_name );
        return VLC_EGENERIC;
    }
    if (GNOME_VFS_FILE_INFO_LOCAL( p_sys->p_file_info ))
    {
        p_sys->b_local = true;
    }
    if( p_sys->p_file_info->type == GNOME_VFS_FILE_TYPE_REGULAR ||
        p_sys->p_file_info->type == GNOME_VFS_FILE_TYPE_CHARACTER_DEVICE ||
        p_sys->p_file_info->type == GNOME_VFS_FILE_TYPE_BLOCK_DEVICE )
    {
        p_sys->b_seekable = true;
    }
    else if( p_sys->p_file_info->type == GNOME_VFS_FILE_TYPE_FIFO
              || p_sys->p_file_info->type == GNOME_VFS_FILE_TYPE_SOCKET )
    {
        p_sys->b_seekable = false;
    }
    else
    {
        msg_Err( p_access, "unknown file type for `%s'", psz_name );
        return VLC_EGENERIC;
    }
    if( p_sys->b_seekable
    && !(p_sys->p_file_info->valid_fields & GNOME_VFS_FILE_INFO_FIELDS_SIZE) )
    {
        /* FIXME that's bad because all others access will be probed */
        msg_Warn( p_access, "file %s size is unknown, aborting", psz_name );
        gnome_vfs_file_info_unref( p_sys->p_file_info );
        gnome_vfs_uri_unref( p_uri);
        free( p_sys );
        free( psz_uri );
        free( psz_name );
        return VLC_EGENERIC;
    }
    free( psz_uri );
    p_sys->psz_name = psz_name;
    gnome_vfs_uri_unref( p_uri);
    return VLC_SUCCESS;
}
