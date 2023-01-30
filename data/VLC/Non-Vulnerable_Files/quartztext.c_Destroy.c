 *****************************************************************************/
static void Destroy(vlc_object_t *p_this)
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
#ifndef TARGET_OS_IPHONE
    if (p_sys->p_fonts) {
        for (int k = 0; k < p_sys->i_fonts; k++)
            ATSFontDeactivate(p_sys->p_fonts[k], NULL, kATSOptionFlagsDefault);
        free(p_sys->p_fonts);
    }
#endif
    free(p_sys->psz_font_name);
    free(p_sys);
}
