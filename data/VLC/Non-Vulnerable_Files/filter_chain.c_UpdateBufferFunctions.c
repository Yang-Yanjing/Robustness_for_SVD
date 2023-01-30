 */
static int UpdateBufferFunctions( filter_chain_t *p_chain )
{
    if( !strcmp( p_chain->psz_capability, "video filter2" ) )
        return UpdateVideoBufferFunctions( p_chain );
    return VLC_SUCCESS;
}
