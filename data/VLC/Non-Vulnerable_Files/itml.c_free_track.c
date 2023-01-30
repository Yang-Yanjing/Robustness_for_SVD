}
static void free_track( track_elem_t *p_track )
{
    fprintf( stderr, "free track\n" );
    if ( !p_track )
        return;
    FREENULL( p_track->name );
    FREENULL( p_track->artist );
    FREENULL( p_track->album );
    FREENULL( p_track->genre );
    FREENULL( p_track->trackNum );
    FREENULL( p_track->location );
    p_track->duration = 0;
    free( p_track );
}
