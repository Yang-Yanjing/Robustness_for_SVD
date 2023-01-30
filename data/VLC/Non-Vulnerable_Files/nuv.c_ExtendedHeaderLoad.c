}
static int ExtendedHeaderLoad( demux_t *p_demux, extended_header_t *h )
{
    uint8_t buffer[512];
    if( stream_Read( p_demux->s, buffer, 512 ) != 512 )
        return VLC_EGENERIC;
    h->i_version = GetDWLE( &buffer[0] );
    h->i_video_fcc = VLC_FOURCC( buffer[4], buffer[5], buffer[6], buffer[7] );
    h->i_audio_fcc = VLC_FOURCC( buffer[8], buffer[9], buffer[10], buffer[11] );
    h->i_audio_sample_rate = GetDWLE( &buffer[12] );
    h->i_audio_bits_per_sample = GetDWLE( &buffer[16] );
    h->i_audio_channels = GetDWLE( &buffer[20] );
    h->i_audio_compression_ratio = GetDWLE( &buffer[24] );
    h->i_audio_quality = GetDWLE( &buffer[28] );
    h->i_rtjpeg_quality = GetDWLE( &buffer[32] );
    h->i_rtjpeg_luma_filter = GetDWLE( &buffer[36] );
    h->i_rtjpeg_chroma_filter = GetDWLE( &buffer[40] );
    h->i_lavc_bitrate = GetDWLE( &buffer[44] );
    h->i_lavc_qmin = GetDWLE( &buffer[48] );
    h->i_lavc_qmin = GetDWLE( &buffer[52] );
    h->i_lavc_maxqdiff = GetDWLE( &buffer[56] );
    h->i_seektable_offset = GetQWLE( &buffer[60] );
    h->i_keyframe_adjust_offset= GetQWLE( &buffer[68] );
#if 0
    msg_Dbg( p_demux, "ex hdr: v=%d vffc=%4.4s afcc=%4.4s %dHz %dbits ach=%d acr=%d aq=%d"
                      "rtjpeg q=%d lf=%d lc=%d lavc br=%d qmin=%d qmax=%d maxqdiff=%d seekableoff=%"PRIi64" keyfao=%"PRIi64,
             h->i_version,
             (char*)&h->i_video_fcc,
             (char*)&h->i_audio_fcc, h->i_audio_sample_rate, h->i_audio_bits_per_sample, h->i_audio_channels,
             h->i_audio_compression_ratio, h->i_audio_quality,
             h->i_rtjpeg_quality, h->i_rtjpeg_luma_filter, h->i_rtjpeg_chroma_filter,
             h->i_lavc_bitrate, h->i_lavc_qmin, h->i_lavc_qmax, h->i_lavc_maxqdiff,
             h->i_seektable_offset, h->i_keyframe_adjust_offset );
#endif
    return VLC_SUCCESS;
}
