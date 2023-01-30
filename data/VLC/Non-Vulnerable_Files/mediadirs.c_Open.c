 *****************************************************************************/
static int Open( vlc_object_t *p_this, enum type_e i_type )
{
    services_discovery_t *p_sd = ( services_discovery_t* )p_this;
    services_discovery_sys_t *p_sys;
    p_sd->p_sys = p_sys = calloc( 1, sizeof( *p_sys) );
    if( !p_sys )
        return VLC_ENOMEM;
    p_sys->i_type = i_type;
    if( p_sys->i_type == Video )
    {
        p_sys->psz_dir[0] = config_GetUserDir( VLC_VIDEOS_DIR );
        p_sys->psz_dir[1] = var_CreateGetString( p_sd, "input-record-path" );
        p_sys->psz_var = "record-file";
    }
    else if( p_sys->i_type == Audio )
    {
        p_sys->psz_dir[0] = config_GetUserDir( VLC_MUSIC_DIR );
        p_sys->psz_dir[1] = var_CreateGetString( p_sd, "input-record-path" );
        p_sys->psz_var = "record-file";
    }
    else if( p_sys->i_type == Picture )
    {
        p_sys->psz_dir[0] = config_GetUserDir( VLC_PICTURES_DIR );
        p_sys->psz_dir[1] = var_CreateGetString( p_sd, "snapshot-path" );
        p_sys->psz_var = "snapshot-file";
    }
    else
    {
        free( p_sys );
        return VLC_EGENERIC;
    }
    var_AddCallback( p_sd->p_libvlc, p_sys->psz_var, onNewFileAdded, p_sd );
    if( vlc_clone( &p_sys->thread, Run, p_sd, VLC_THREAD_PRIORITY_LOW ) )
    {
        var_DelCallback( p_sd->p_libvlc, p_sys->psz_var, onNewFileAdded, p_sd );
        free( p_sys->psz_dir[1] );
        free( p_sys->psz_dir[0] );
        free( p_sys );
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
