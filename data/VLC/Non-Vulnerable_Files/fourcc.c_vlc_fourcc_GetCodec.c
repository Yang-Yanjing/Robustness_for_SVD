/* */
vlc_fourcc_t vlc_fourcc_GetCodec( int i_cat, vlc_fourcc_t i_fourcc )
{
    entry_t e = Find( i_cat, i_fourcc );
    if( CreateFourcc( e.p_class ) == 0 )
        return i_fourcc;
    return CreateFourcc( e.p_class );
}
