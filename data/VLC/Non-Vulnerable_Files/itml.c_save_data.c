}
static bool save_data( track_elem_t *p_track, const char *psz_name,
                       char *psz_value)
{
    /* exit if setting is impossible */
    if( !psz_name || !psz_value || !p_track )
        return false;
    /* re-convert xml special characters inside psz_value */
    resolve_xml_special_chars( psz_value );
#define SAVE_INFO( name, value ) \
    if( !strcmp( psz_name, name ) ) { p_track->value = strdup( psz_value ); }
    SAVE_INFO( "Name", name )
    else SAVE_INFO( "Artist", artist )
    else SAVE_INFO( "Album", album )
    else SAVE_INFO( "Genre", genre )
    else SAVE_INFO( "Track Number", trackNum )
    else SAVE_INFO( "Location", location )
    else if( !strcmp( psz_name, "Total Time" ) )
    {
        long i_num = atol( psz_value );
        p_track->duration = (mtime_t) i_num*1000;
    }
#undef SAVE_INFO
    return true;
}
