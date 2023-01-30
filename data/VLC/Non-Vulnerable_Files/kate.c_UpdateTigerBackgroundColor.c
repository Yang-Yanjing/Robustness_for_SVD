}
static void UpdateTigerBackgroundColor( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = (decoder_sys_t*)p_dec->p_sys;
    double f_a = ((p_sys->i_tiger_default_background_color >> 24) & 0xff) / 255.0;
    double f_r = ((p_sys->i_tiger_default_background_color >> 16) & 0xff) / 255.0;
    double f_g = ((p_sys->i_tiger_default_background_color >> 8) & 0xff) / 255.0;
    double f_b = (p_sys->i_tiger_default_background_color & 0xff) / 255.0;
    CHECK_TIGER_RET( tiger_renderer_set_default_background_fill_color( p_sys->p_tr, f_r, f_g, f_b, f_a ) );
    p_sys->b_dirty = true;
}
