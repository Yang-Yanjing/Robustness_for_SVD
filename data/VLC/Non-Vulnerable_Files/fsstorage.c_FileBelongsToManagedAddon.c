}
static bool FileBelongsToManagedAddon( addons_finder_t *p_finder,
                                       const addon_type_t e_type,
                                       const char *psz_file )
{
    FOREACH_ARRAY( const addon_entry_t *p_entry, p_finder->entries )
        if ( ( p_entry->e_flags & ADDON_MANAGEABLE ) == 0 )
            continue;
        FOREACH_ARRAY( const addon_file_t *p_file, p_entry->files )
            if ( p_file->e_filetype == e_type
                 && !strcmp( p_file->psz_filename, psz_file ) )
                return true;
        FOREACH_END();
    FOREACH_END();
    return false;
}
