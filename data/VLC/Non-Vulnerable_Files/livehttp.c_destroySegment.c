}
static void destroySegment( output_segment_t *segment )
{
    free( segment->psz_filename );
    free( segment->psz_duration );
    free( segment->psz_uri );
    free( segment->psz_key_uri );
    free( segment );
}
