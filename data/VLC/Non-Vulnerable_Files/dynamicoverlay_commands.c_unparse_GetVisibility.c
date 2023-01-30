}
static int unparse_GetVisibility( const commandparams_t *p_results,
                             buffer_t *p_output )
{
    int ret = BufferPrintf( p_output, " %d", (p_results->b_visible ? 1 : 0) );
    if( ret != VLC_SUCCESS ) {
        return ret;
    }
    return VLC_SUCCESS;
}
