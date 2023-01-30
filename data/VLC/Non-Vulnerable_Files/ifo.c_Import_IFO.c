 *****************************************************************************/
int Import_IFO( vlc_object_t *p_this )
{
    demux_t *p_demux = (demux_t *)p_this;
    if( !p_demux->psz_file )
        return VLC_EGENERIC;
    size_t len = strlen( p_demux->psz_file );
    char *psz_file = p_demux->psz_file + len - strlen( "VIDEO_TS.IFO" );
    /* Valid filenames are :
     *  - VIDEO_TS.IFO
     *  - VTS_XX_X.IFO where X are digits
     */
    if( len > strlen( "VIDEO_TS.IFO" )
        && ( !strcasecmp( psz_file, "VIDEO_TS.IFO" )
        || (!strncasecmp( psz_file, "VTS_", 4 )
        && !strcasecmp( psz_file + strlen( "VTS_00_0" ) , ".IFO" ) ) ) )
    {
        int i_peek;
        const uint8_t *p_peek;
        i_peek = stream_Peek( p_demux->s, &p_peek, 8 );
        if( i_peek != 8 || memcmp( p_peek, "DVDVIDEO", 8 ) )
            return VLC_EGENERIC;
        p_demux->pf_demux = Demux;
    }
    /* Valid filename for DVD-VR is VR_MANGR.IFO */
    else if( len >= 12 && !strcmp( &p_demux->psz_file[len-12], "VR_MANGR.IFO" ) )
    {
        int i_peek;
        const uint8_t *p_peek;
        i_peek = stream_Peek( p_demux->s, &p_peek, 8 );
        if( i_peek != 8 || memcmp( p_peek, "DVD_RTR_", 8 ) )
            return VLC_EGENERIC;
        p_demux->pf_demux = DemuxDVD_VR;
    }
    else
        return VLC_EGENERIC;
//    STANDARD_DEMUX_INIT_MSG( "found valid VIDEO_TS.IFO" )
    p_demux->pf_control = Control;
    return VLC_SUCCESS;
}
