 *****************************************************************************/
static int DemuxOpen ( vlc_object_t *p_this )
{
    demux_t *p_demux = (demux_t*)p_this;
    dvdnav_t *p_dvdnav = NULL;
    int i_ret = VLC_EGENERIC;
    int64_t i_init_pos;
    bool forced = false, b_seekable = false;
    if( p_demux->psz_demux != NULL
     && !strncmp(p_demux->psz_demux, "dvd", 3) )
        forced = true;
    /* StreamProbeDVD need FASTSEEK, but if dvd is forced, we don't probe thus
     * don't need fastseek */
    stream_Control( p_demux->s, forced ? STREAM_CAN_SEEK : STREAM_CAN_FASTSEEK,
                    &b_seekable );
    if( !b_seekable )
        return VLC_EGENERIC;
    i_init_pos = stream_Tell( p_demux->s );
    /* Try some simple probing to avoid going through dvdnav_open too often */
    if( !forced && StreamProbeDVD( p_demux->s ) != VLC_SUCCESS )
        goto bailout;
    static dvdnav_stream_cb stream_cb =
    {
        .pf_seek = stream_cb_seek,
        .pf_read = stream_cb_read,
        .pf_readv = NULL,
    };
    /* Open dvdnav with stream callbacks */
    if( dvdnav_open_stream( &p_dvdnav, p_demux->s,
                            &stream_cb ) != DVDNAV_STATUS_OK )
    {
        msg_Warn( p_demux, "cannot open DVD with open_stream" );
        goto bailout;
    }
    i_ret = CommonOpen( p_this, p_dvdnav, false );
    if( i_ret != VLC_SUCCESS )
        dvdnav_close( p_dvdnav );
bailout:
    if( i_ret != VLC_SUCCESS )
        stream_Seek( p_demux->s, i_init_pos );
    return i_ret;
}
