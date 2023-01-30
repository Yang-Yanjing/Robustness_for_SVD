}
static int unparse_GetAlpha( const commandparams_t *p_results,
                             buffer_t *p_output )
{
    int ret = BufferPrintf( p_output, " %d", p_results->i_alpha );
    if( ret != VLC_SUCCESS )
        return ret;
    return VLC_SUCCESS;
}
