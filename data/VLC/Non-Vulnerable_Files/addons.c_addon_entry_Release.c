}
void addon_entry_Release( addon_entry_t * p_entry )
{
    addon_entry_owner_t *owner = (addon_entry_owner_t *) p_entry;
    if( atomic_fetch_sub(&owner->refs, 1) != 1 )
        return;
    free( p_entry->psz_name );
    free( p_entry->psz_summary );
    free( p_entry->psz_description );
    free( p_entry->psz_archive_uri );
    free( p_entry->psz_author );
    free( p_entry->psz_source_uri );
    free( p_entry->psz_image_uri );
    free( p_entry->psz_image_data );
    free( p_entry->psz_source_module );
    free( p_entry->psz_version );
    free( p_entry->p_custom );
    addon_file_t *p_file;
    FOREACH_ARRAY( p_file, p_entry->files )
    free( p_file->psz_filename );
    free( p_file->psz_download_uri );
    FOREACH_END()
    ARRAY_RESET( p_entry->files );
    vlc_mutex_destroy( &p_entry->lock );
    free( owner );
}
