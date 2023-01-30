}
static void UpdateTigerFontEffect( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = (decoder_sys_t*)p_dec->p_sys;
    CHECK_TIGER_RET( tiger_renderer_set_default_font_effect( p_sys->p_tr,
                                                             p_sys->e_tiger_default_font_effect,
                                                             p_sys->f_tiger_default_font_effect_strength ) );
    p_sys->b_dirty = true;
}
