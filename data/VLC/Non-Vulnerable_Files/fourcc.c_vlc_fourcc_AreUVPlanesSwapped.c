}
bool vlc_fourcc_AreUVPlanesSwapped( vlc_fourcc_t a, vlc_fourcc_t b )
{
    static const vlc_fourcc_t pp_swapped[][4] = {
        { VLC_CODEC_YV12, VLC_CODEC_I420, VLC_CODEC_J420, 0 },
        { VLC_CODEC_YV9,  VLC_CODEC_I410, 0 },
        { 0 }
    };
    for( int i = 0; pp_swapped[i][0]; i++ )
    {
        if( pp_swapped[i][0] == b )
        {
            vlc_fourcc_t t = a;
            a = b;
            b = t;
        }
        if( pp_swapped[i][0] != a )
            continue;
        for( int j = 1; pp_swapped[i][j]; j++ )
        {
            if( pp_swapped[i][j] == b )
                return true;
        }
    }
    return false;
}
