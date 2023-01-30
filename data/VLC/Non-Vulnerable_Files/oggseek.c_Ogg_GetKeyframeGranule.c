}
int64_t Ogg_GetKeyframeGranule( logical_stream_t *p_stream, int64_t i_granule )
{
    if ( p_stream->b_oggds )
    {
           return -1; /* We have no way to know */
    }
    else if( p_stream->fmt.i_codec == VLC_CODEC_THEORA )
    {
        return ( i_granule >> p_stream->i_granule_shift ) << p_stream->i_granule_shift;
    }
    else if( p_stream->fmt.i_codec == VLC_CODEC_DIRAC )
    {
        return ( i_granule >> 31 ) << 31;
    }
    /* No change, that's keyframe or it can't be shifted out (oggds) */
    return i_granule;
}
