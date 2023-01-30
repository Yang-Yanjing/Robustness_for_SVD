 * XXX it assumes that the string is at least four bytes */
static inline vlc_fourcc_t CreateFourcc( const char *psz_fourcc )
{
    return VLC_FOURCC( psz_fourcc[0], psz_fourcc[1],
                       psz_fourcc[2], psz_fourcc[3] );
}
