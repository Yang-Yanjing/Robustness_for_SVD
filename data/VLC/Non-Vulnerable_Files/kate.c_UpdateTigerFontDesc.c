}
static void UpdateTigerFontDesc( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = (decoder_sys_t*)p_dec->p_sys;
    CHECK_TIGER_RET( tiger_renderer_set_default_font_description( p_sys->p_tr, p_sys->psz_tiger_default_font_desc ) );
    p_sys->b_dirty = true;
}
