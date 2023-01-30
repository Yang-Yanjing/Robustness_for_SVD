}
static inline void DecoderUpdatePreroll( int64_t *pi_preroll, const block_t *p )
{
    if( p->i_flags & (BLOCK_FLAG_PREROLL|BLOCK_FLAG_DISCONTINUITY) )
        *pi_preroll = INT64_MAX;
    else if( p->i_dts > VLC_TS_INVALID )
        *pi_preroll = __MIN( *pi_preroll, p->i_dts );
    else if( p->i_pts > VLC_TS_INVALID )
        *pi_preroll = __MIN( *pi_preroll, p->i_pts );
}
