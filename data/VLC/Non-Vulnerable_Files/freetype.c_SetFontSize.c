}
static int SetFontSize( filter_t *p_filter, int i_size )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    if( !i_size )
    {
        i_size = GetFontSize( p_filter );
        msg_Dbg( p_filter, "using fontsize: %i", i_size );
    }
    p_sys->style.i_font_size = i_size;
    if( FT_Set_Pixel_Sizes( p_sys->p_face, 0, i_size ) )
    {
        msg_Err( p_filter, "couldn't set font size to %d", i_size );
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
