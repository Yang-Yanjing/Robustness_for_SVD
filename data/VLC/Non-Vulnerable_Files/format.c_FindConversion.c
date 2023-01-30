};
static cvt_t FindConversion(vlc_fourcc_t src, vlc_fourcc_t dst)
{
    for (int i = 0; cvt_directs[i].convert; i++) {
        if (cvt_directs[i].src == src &&
            cvt_directs[i].dst == dst)
            return cvt_directs[i].convert;
    }
    return NULL;
}
