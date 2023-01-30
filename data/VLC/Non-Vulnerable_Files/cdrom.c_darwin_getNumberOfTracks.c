 ****************************************************************************/
static int darwin_getNumberOfTracks( CDTOC *pTOC, int i_descriptors )
{
    u_char track;
    int i, i_tracks = 0;
    CDTOCDescriptor *pTrackDescriptors = NULL;
    pTrackDescriptors = (CDTOCDescriptor *)pTOC->descriptors;
    for( i = i_descriptors; i > 0; i-- )
    {
        track = pTrackDescriptors[i].point;
        if( track > CD_MAX_TRACK_NO || track < CD_MIN_TRACK_NO )
            continue;
        i_tracks++;
    }
    return( i_tracks );
}
