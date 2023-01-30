 */
static bool add_meta( input_item_t *p_input_item, track_elem_t *p_track )
{
    /* exit if setting is impossible */
    if( !p_input_item || !p_track )
        return false;
#define SET_INFO( type, prop ) \
    if( p_track->prop ) {input_item_Set##type( p_input_item, p_track->prop );}
    SET_INFO( Title, name )
    SET_INFO( Artist, artist )
    SET_INFO( Album, album )
    SET_INFO( Genre, genre )
    SET_INFO( TrackNum, trackNum )
    SET_INFO( Duration, duration )
#undef SET_INFO
    return true;
}
