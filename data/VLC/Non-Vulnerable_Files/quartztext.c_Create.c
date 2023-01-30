 *****************************************************************************/
static int Create(vlc_object_t *p_this)
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    // Allocate structure
    p_filter->p_sys = p_sys = malloc(sizeof(filter_sys_t));
    if (!p_sys)
        return VLC_ENOMEM;
    p_sys->psz_font_name  = var_CreateGetString(p_this, "quartztext-font");
    p_sys->i_font_opacity = 255;
    p_sys->i_font_color = VLC_CLIP(var_CreateGetInteger(p_this, "quartztext-color") , 0, 0xFFFFFF);
    p_sys->i_font_size = GetFontSize(p_filter);
    p_filter->pf_render_text = RenderText;
    p_filter->pf_render_html = RenderHtml;
#ifndef TARGET_OS_IPHONE
    p_sys->p_fonts = NULL;
    p_sys->i_fonts = 0;
#endif
    LoadFontsFromAttachments(p_filter);
    return VLC_SUCCESS;
}
