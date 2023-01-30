}
static int unparse_GetPosition( const commandparams_t *p_results,
                                buffer_t *p_output )
{
    int ret = BufferPrintf( p_output, " %d", p_results->i_x );
    if( ret != VLC_SUCCESS )
        return ret;
    ret = BufferPrintf( p_output, " %d", p_results->i_y );
    if( ret != VLC_SUCCESS )
        return ret;
    return VLC_SUCCESS;
}
