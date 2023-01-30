}
static inline vlc_fourcc_t ReadFOURCC( const uint8_t *p )
{
    return VLC_FOURCC( p[0], p[1], p[2], p[3] );
}
