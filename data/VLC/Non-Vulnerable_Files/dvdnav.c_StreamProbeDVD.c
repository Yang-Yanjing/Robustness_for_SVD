 *****************************************************************************/
static int StreamProbeDVD( stream_t *s )
{
    /* ISO 9660 volume descriptor */
    char iso_dsc[6];
    if( stream_Seek( s, 0x8000 + 1 ) != VLC_SUCCESS
     || stream_Read( s, iso_dsc, sizeof (iso_dsc) ) < (int)sizeof (iso_dsc)
     || memcmp( iso_dsc, "CD001\x01", 6 ) )
        return VLC_EGENERIC;
    /* Try to find the anchor (2 bytes at LBA 256) */
    uint16_t anchor;
    if( stream_Seek( s, 256 * DVD_VIDEO_LB_LEN ) == VLC_SUCCESS
     && stream_Read( s, &anchor, 2 ) == 2
     && GetWLE( &anchor ) == 2 )
        return VLC_SUCCESS;
    else
        return VLC_EGENERIC;
}
