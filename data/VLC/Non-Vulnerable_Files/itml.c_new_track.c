}
static track_elem_t *new_track()
{
    track_elem_t *p_track;
    p_track = malloc( sizeof( track_elem_t ) );
    if( p_track )
    {
        p_track->name = NULL;
        p_track->artist = NULL;
        p_track->album = NULL;
        p_track->genre = NULL;
        p_track->trackNum = NULL;
        p_track->location = NULL;
        p_track->duration = 0;
    }
    return p_track;
}
