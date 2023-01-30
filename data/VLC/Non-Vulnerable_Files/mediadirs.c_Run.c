 *****************************************************************************/
static void *Run( void *data )
{
    services_discovery_t *p_sd = data;
    services_discovery_sys_t *p_sys = p_sd->p_sys;
    int canc = vlc_savecancel();
    int num_dir = sizeof( p_sys->psz_dir ) / sizeof( p_sys->psz_dir[0] );
    for( int i = 0; i < num_dir; i++ )
    {
        char* psz_dir = p_sys->psz_dir[i];
        /* make sure the directory exists */
        struct stat st;
        if( psz_dir == NULL            ||
            vlc_stat( psz_dir, &st )  ||
            !S_ISDIR( st.st_mode ) )
            continue;
        char* psz_uri = vlc_path2uri( psz_dir, "file" );
        input_item_t* p_root = input_item_New( psz_uri, NULL );
        if( p_sys->i_type == Picture )
            input_item_AddOption( p_root, "ignore-filetypes=ini,db,lnk,txt",
                                  VLC_INPUT_OPTION_TRUSTED );
        input_item_AddOption( p_root, "recursive=collapse",
                              VLC_INPUT_OPTION_TRUSTED );
        vlc_event_manager_t *p_em = &p_root->event_manager;
        vlc_event_attach( p_em, vlc_InputItemSubItemAdded,
                          input_item_subitem_added, p_sd );
        input_Read( p_sd, p_root );
        vlc_event_detach( p_em, vlc_InputItemSubItemAdded,
                          input_item_subitem_added, p_sd );
        vlc_gc_decref( p_root );
        free( psz_uri );
    }
    vlc_restorecancel(canc);
    return NULL;
}
