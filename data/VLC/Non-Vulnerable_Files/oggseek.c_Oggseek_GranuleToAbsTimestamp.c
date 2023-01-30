/* Dont use b_presentation with frames granules ! */
int64_t Oggseek_GranuleToAbsTimestamp( logical_stream_t *p_stream,
                                       int64_t i_granule, bool b_presentation )
{
    int64_t i_timestamp = -1;
    if ( i_granule < 1 )
        return -1;
    if ( p_stream->b_oggds )
    {
        if ( b_presentation ) i_granule--;
        i_timestamp = i_granule * CLOCK_FREQ / p_stream->f_rate;
    }
    else  switch( p_stream->fmt.i_codec )
    {
    case VLC_CODEC_THEORA:
    case VLC_CODEC_KATE:
    {
        ogg_int64_t iframe = i_granule >> p_stream->i_granule_shift;
        ogg_int64_t pframe = i_granule - ( iframe << p_stream->i_granule_shift );
        /* See Theora A.2.3 */
        if ( b_presentation ) pframe -= p_stream->i_keyframe_offset;
        i_timestamp = ( iframe + pframe ) * CLOCK_FREQ / p_stream->f_rate;
        break;
    }
    case VLC_CODEC_VP8:
    {
        ogg_int64_t frame = i_granule >> p_stream->i_granule_shift;
        if ( b_presentation ) frame--;
        i_timestamp = frame * CLOCK_FREQ / p_stream->f_rate;
        break;
    }
    case VLC_CODEC_DIRAC:
    {
        ogg_int64_t i_dts = i_granule >> 31;
        ogg_int64_t delay = (i_granule >> 9) & 0x1fff;
        /* NB, OggDirac granulepos values are in units of 2*picturerate */
        double f_rate = p_stream->f_rate;
        if ( !p_stream->special.dirac.b_interlaced ) f_rate *= 2;
        if ( b_presentation ) i_dts += delay;
        i_timestamp = i_dts * CLOCK_FREQ / f_rate;
        break;
    }
    case VLC_CODEC_OPUS:
    {
        if ( b_presentation ) return VLC_TS_INVALID;
        i_timestamp = ( i_granule - p_stream->i_pre_skip ) * CLOCK_FREQ / 48000;
        break;
    }
    case VLC_CODEC_VORBIS:
    case VLC_CODEC_FLAC:
    {
        if ( b_presentation ) return VLC_TS_INVALID;
        i_timestamp = i_granule * CLOCK_FREQ / p_stream->f_rate;
        break;
    }
    case VLC_CODEC_SPEEX:
    {
        if ( b_presentation )
            i_granule -= p_stream->special.speex.i_framesize *
                         p_stream->special.speex.i_framesperpacket;
        i_timestamp = i_granule * CLOCK_FREQ / p_stream->f_rate;
        break;
    }
    }
    return i_timestamp;
}
