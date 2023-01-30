}
static int DemuxDVD_VR( demux_t *p_demux )
{
    size_t len = strlen( p_demux->psz_location );
    char *psz_url = malloc( len + 1 );
    if( unlikely( psz_url == NULL ) )
        return 0;
    assert( len >= 12 );
    len -= 12;
    memcpy( psz_url, p_demux->psz_location, len );
    memcpy( psz_url + len, "VR_MOVIE.VRO", 13 );
    input_item_t *p_current_input = GetCurrentItem(p_demux);
    input_item_t *p_input = input_item_New( psz_url, psz_url );
    input_item_PostSubItem( p_current_input, p_input );
    vlc_gc_decref( p_input );
    vlc_gc_decref(p_current_input);
    free( psz_url );
    return 0; /* Needed for correct operation of go back */
}
