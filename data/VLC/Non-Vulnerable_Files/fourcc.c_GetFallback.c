/* */
static const vlc_fourcc_t *GetFallback( vlc_fourcc_t i_fourcc,
                                        const vlc_fourcc_t *pp_fallback[],
                                        const vlc_fourcc_t p_list[] )
{
    for( unsigned i = 0; pp_fallback[i]; i++ )
    {
        if( pp_fallback[i][0] == i_fourcc )
            return pp_fallback[i];
    }
    return p_list;
}
