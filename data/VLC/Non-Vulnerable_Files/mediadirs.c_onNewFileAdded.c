}
static int onNewFileAdded( vlc_object_t *p_this, char const *psz_var,
                     vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    (void)p_this;
    services_discovery_t *p_sd = p_data;
    services_discovery_sys_t *p_sys = p_sd->p_sys;
    (void)psz_var; (void)oldval;
    char* psz_file = newval.psz_string;
    if( !psz_file || !*psz_file )
        return VLC_EGENERIC;
    char* psz_uri = vlc_path2uri( psz_file, "file" );
    input_item_t* p_item = input_item_New( psz_uri, NULL );
    if( p_sys->i_type == Picture )
    {
        if( fileType( p_sd, psz_file ) == Picture )
        {
            formatSnapshotItem( p_item );
            services_discovery_AddItem( p_sd, p_item, NULL );
            msg_Dbg( p_sd, "New snapshot added : %s", psz_file );
        }
    }
    else if( p_sys->i_type == Audio )
    {
        if( fileType( p_sd, psz_file ) == Audio )
        {
            services_discovery_AddItem( p_sd, p_item, NULL );
            msg_Dbg( p_sd, "New recorded audio added : %s", psz_file );
        }
    }
    else if( p_sys->i_type == Video )
    {
        if( fileType( p_sd, psz_file ) == Video ||
            fileType( p_sd, psz_file ) == Unknown )
        {
            services_discovery_AddItem( p_sd, p_item, NULL );
            msg_Dbg( p_sd, "New recorded video added : %s", psz_file );
        }
    }
    vlc_gc_decref( p_item );
    free( psz_uri );
    return VLC_SUCCESS;
}
