}
static int exec_GetTextSize( filter_t *p_filter,
                             const commandparams_t *p_params,
                             commandparams_t *p_results )
{
    filter_sys_t *p_sys = (filter_sys_t*) p_filter->p_sys;
    overlay_t *p_ovl = ListGet( &p_sys->overlays, p_params->i_id );
    if( p_ovl == NULL )
        return VLC_EGENERIC;
    p_results->fontstyle.i_font_size = p_ovl->p_fontstyle->i_font_size;
    return VLC_SUCCESS;
}
