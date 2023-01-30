}
static int Demux( demux_t *p_demux )
{
    char *psz_url, *psz_dir;
    psz_dir = strrchr( p_demux->psz_location, '/' );
    if( psz_dir != NULL )
       psz_dir[1] = '\0';
    if( asprintf( &psz_url, "dvd://%s", p_demux->psz_location ) == -1 )
        return 0;
    input_item_t *p_current_input = GetCurrentItem(p_demux);
    input_item_t *p_input = input_item_New( psz_url, psz_url );
    input_item_PostSubItem( p_current_input, p_input );
    vlc_gc_decref( p_input );
    vlc_gc_decref(p_current_input);
    free( psz_url );
    return 0; /* Needed for correct operation of go back */
}
