}\
static int WriteCatalog( addons_storage_t *p_storage,
                         addon_entry_t **pp_entries, int i_entries )
{
    addon_entry_t *p_entry;
    char *psz_file;
    char *psz_file_tmp;
    char *psz_tempstring;
    char *psz_userdir = config_GetUserDir( VLC_DATA_DIR );
    if ( !psz_userdir ) return VLC_ENOMEM;
    if ( asprintf( &psz_file, "%s%s", psz_userdir, ADDONS_CATALOG ) < 1 )
    {
        free( psz_userdir );
        return VLC_ENOMEM;
    }
    free( psz_userdir );
    if ( asprintf( &psz_file_tmp, "%s.tmp%"PRIu32, psz_file, (uint32_t)getpid() ) < 1 )
    {
        free( psz_file );
        return VLC_ENOMEM;
    }
    char *psz_path = strdup( psz_file );
    if ( !psz_path )
    {
        free( psz_file );
        free( psz_file_tmp );
        return VLC_ENOMEM;
    }
    char *psz_buf = strrchr( psz_path, DIR_SEP_CHAR );
    if( psz_buf )
    {
        *++psz_buf = '\0';
        /* ensure directory exists */
        if( !EMPTY_STR( psz_path ) ) recursive_mkdir( VLC_OBJECT(p_storage), psz_path );
    }
    free( psz_path );
    FILE *p_catalog = vlc_fopen( psz_file_tmp, "wt" );
    if ( !p_catalog )
    {
        free( psz_file );
        free( psz_file_tmp );
        return VLC_EGENERIC;
    }
    /* write XML header */
    fprintf( p_catalog, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
    fprintf( p_catalog, "<videolan xmlns=\"http://videolan.org/ns/vlc/addons/1.0\">\n" );
    fprintf( p_catalog, "\t<addons>\n" );
    for ( int i=0; i<i_entries; i++ )
    {
        p_entry = pp_entries[i];
        vlc_mutex_lock( &p_entry->lock );
        psz_tempstring = NULL;
        if ( ( p_entry->e_state != ADDON_INSTALLED ) ||
             !( p_entry->e_flags & ADDON_MANAGEABLE ) )
        {
            vlc_mutex_unlock( &p_entry->lock );
            continue;
        }
        if ( p_entry->psz_source_module )
            psz_tempstring = convert_xml_special_chars( p_entry->psz_source_module );
        char *psz_uuid = addons_uuid_to_psz( ( const addon_uuid_t * ) & p_entry->uuid );
        fprintf( p_catalog, "\t\t<addon source=\"%s\" type=\"%s\" id=\"%s\" "
                                 "downloads=\"%ld\" score=\"%d\"",
                 ( psz_tempstring ) ? psz_tempstring : "",
                 getTypePsz( p_entry->e_type ),
                 psz_uuid,
                 p_entry->i_downloads,
                 p_entry->i_score );
        free( psz_uuid );
        free( psz_tempstring );
        WRITE_WITH_ENTITIES( " version=\"%s\"", p_entry->psz_version )
        fprintf( p_catalog, ">\n" );
        WRITE_WITH_ENTITIES( "\t\t\t<name>%s</name>\n", p_entry->psz_name )
        WRITE_WITH_ENTITIES( "\t\t\t<summary>%s</summary>\n", p_entry->psz_summary )
        if ( p_entry->psz_description )
        {
            psz_tempstring = p_entry->psz_description;
            /* FIXME: do real escaping */
            while( ( psz_tempstring = strstr( psz_tempstring, "]]>" ) ) )
                *psz_tempstring = ' ';
            fprintf( p_catalog, "\t\t\t<description><![CDATA[%s]]></description>\n", p_entry->psz_description );
        }
        WRITE_WITH_ENTITIES( "\t\t\t<image>%s</image>\n", p_entry->psz_image_data )
        WRITE_WITH_ENTITIES( "\t\t\t<archive>%s</archive>\n", p_entry->psz_archive_uri )
        fprintf( p_catalog, "\t\t\t<authorship>\n" );
        WRITE_WITH_ENTITIES( "\t\t\t\t<creator>%s</creator>\n", p_entry->psz_author )
        WRITE_WITH_ENTITIES( "\t\t\t\t<sourceurl>%s</sourceurl>\n", p_entry->psz_source_uri )
        fprintf( p_catalog, "\t\t\t</authorship>\n" );
        FOREACH_ARRAY( addon_file_t *p_file, p_entry->files )
            psz_tempstring = convert_xml_special_chars( p_file->psz_filename );
            fprintf( p_catalog, "\t\t\t<resource type=\"%s\">%s</resource>\n",
                     getTypePsz( p_file->e_filetype ), psz_tempstring );
            free( psz_tempstring );
        FOREACH_END();
        fprintf( p_catalog, "\t\t</addon>\n" );
        vlc_mutex_unlock( &p_entry->lock );
    }
    fprintf( p_catalog, "\t</addons>\n" );
    fprintf( p_catalog, "</videolan>\n" );
    fclose( p_catalog );
    int i_ret = vlc_rename( psz_file_tmp, psz_file );
    free( psz_file );
    free( psz_file_tmp );
    if( i_ret == -1 )
    {
        msg_Err( p_storage, "could not rename temp catalog: %s",
                 vlc_strerror_c(errno) );
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
