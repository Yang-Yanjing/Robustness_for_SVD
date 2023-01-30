 */
static int Filter( video_splitter_t *p_splitter,
                   picture_t *pp_dst[], picture_t *p_src )
{
    if( video_splitter_NewPicture( p_splitter, pp_dst ) )
    {
        picture_Release( p_src );
        return VLC_EGENERIC;
    }
    for( int i = 0; i < p_splitter->i_output; i++ )
        picture_Copy( pp_dst[i], p_src );
    picture_Release( p_src );
    return VLC_SUCCESS;
}
