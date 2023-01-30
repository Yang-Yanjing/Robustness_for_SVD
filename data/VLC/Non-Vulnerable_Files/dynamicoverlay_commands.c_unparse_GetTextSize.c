}
static int unparse_GetTextSize( const commandparams_t *p_results,
                                buffer_t *p_output )
{
    int ret = BufferPrintf( p_output, " %d", p_results->fontstyle.i_font_size );
    if( ret != VLC_SUCCESS )
        return ret;
    return VLC_SUCCESS;
}
