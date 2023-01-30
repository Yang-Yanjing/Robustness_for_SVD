}
static int unparse_GetTextColor( const commandparams_t *p_results,
                                 buffer_t *p_output )
{
    int ret = BufferPrintf( p_output, " %d", (p_results->fontstyle.i_font_color & 0xff0000)>>16 );
    if( ret != VLC_SUCCESS )
        return ret;
    ret = BufferPrintf( p_output, " %d", (p_results->fontstyle.i_font_color & 0x00ff00)>>8 );
    if( ret != VLC_SUCCESS )
        return ret;
    ret = BufferPrintf( p_output, " %d", (p_results->fontstyle.i_font_color & 0x0000ff) );
    if( ret != VLC_SUCCESS )
        return ret;
    return VLC_SUCCESS;
}
