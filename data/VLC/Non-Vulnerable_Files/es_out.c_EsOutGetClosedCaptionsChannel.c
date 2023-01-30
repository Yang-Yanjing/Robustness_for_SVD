};
static inline int EsOutGetClosedCaptionsChannel( vlc_fourcc_t fcc )
{
    int i;
    for( i = 0; i < 4; i++ )
    {
        if( fcc == EsOutFourccClosedCaptions[i] )
            return i;
    }
    return -1;
}
