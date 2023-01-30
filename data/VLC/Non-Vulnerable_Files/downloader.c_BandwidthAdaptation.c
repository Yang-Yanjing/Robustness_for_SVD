#ifdef DISABLE_BANDWIDTH_ADAPTATION
static unsigned BandwidthAdaptation( stream_t *s,
        sms_stream_t *sms, uint64_t *bandwidth )
{
    return sms->download_qlvl;
}
#else
static unsigned BandwidthAdaptation( stream_t *s,
        sms_stream_t *sms, uint64_t bandwidth )
{
    if( sms->type != VIDEO_ES )
        return sms->download_qlvl;
    uint64_t bw_candidate = 0;
    quality_level_t *qlevel;
    unsigned ret = sms->download_qlvl;
    for( unsigned i = 0; i < sms->qlevel_nb; i++ )
    {
        qlevel = vlc_array_item_at_index( sms->qlevels, i );
        if( unlikely( !qlevel ) )
        {
            msg_Err( s, "Could no get %uth quality level", i );
            return 0;
        }
        if( qlevel->Bitrate < (bandwidth - bandwidth / 3) &&
                qlevel->Bitrate > bw_candidate )
        {
            bw_candidate = qlevel->Bitrate;
            ret = qlevel->id;
        }
    }
    return ret;
}
