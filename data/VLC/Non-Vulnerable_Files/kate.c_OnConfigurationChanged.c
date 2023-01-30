}
static int OnConfigurationChanged( decoder_t *p_dec, const char *psz_var,
                                   vlc_value_t oldval, vlc_value_t newval )
{
    decoder_sys_t *p_sys = (decoder_sys_t*)p_dec->p_sys;
    VLC_UNUSED( oldval );
    vlc_mutex_lock( &p_sys->lock );
    msg_Dbg( p_dec, "OnConfigurationChanged: %s", psz_var );
    if( !p_sys->b_use_tiger || !p_sys->p_tr )
    {
        vlc_mutex_unlock( &p_sys->lock );
        return VLC_SUCCESS;
    }
#define TEST_TIGER_VAR( name ) \
    if( !strcmp( name, psz_var ) )
    TEST_TIGER_VAR( "kate-tiger-quality" )
    {
        p_sys->f_tiger_quality = newval.f_float;
        UpdateTigerQuality( p_dec );
    }
    TEST_TIGER_VAR( "kate-tiger-default-font-desc" )
    {
        if( p_sys->psz_tiger_default_font_desc )
        {
            free( p_sys->psz_tiger_default_font_desc );
            p_sys->psz_tiger_default_font_desc = NULL;
        }
        if( newval.psz_string )
        {
            p_sys->psz_tiger_default_font_desc = strdup( newval.psz_string );
        }
        UpdateTigerFontDesc( p_dec );
    }
    TEST_TIGER_VAR( "kate-tiger-default-font-color" )
    {
        p_sys->i_tiger_default_font_color = (p_sys->i_tiger_default_font_color & 0xff00000000) | newval.i_int;
        UpdateTigerFontColor( p_dec );
    }
    TEST_TIGER_VAR( "kate-tiger-default-font-alpha" )
    {
        p_sys->i_tiger_default_font_color = (p_sys->i_tiger_default_font_color & 0x00ffffff) | (newval.i_int<<24);
        UpdateTigerFontColor( p_dec );
    }
    TEST_TIGER_VAR( "kate-tiger-default-background-color" )
    {
        p_sys->i_tiger_default_background_color = (p_sys->i_tiger_default_background_color & 0xff00000000) | newval.i_int;
        UpdateTigerBackgroundColor( p_dec );
    }
    TEST_TIGER_VAR( "kate-tiger-default-background-alpha" )
    {
        p_sys->i_tiger_default_background_color = (p_sys->i_tiger_default_background_color & 0x00ffffff) | (newval.i_int<<24);
        UpdateTigerBackgroundColor( p_dec );
    }
    TEST_TIGER_VAR( "kate-tiger-default-font-effect" )
    {
        p_sys->e_tiger_default_font_effect = (tiger_font_effect)newval.i_int;
        UpdateTigerFontEffect( p_dec );
    }
    TEST_TIGER_VAR( "kate-tiger-default-font-effect-strength" )
    {
        p_sys->f_tiger_default_font_effect_strength = newval.f_float;
        UpdateTigerFontEffect( p_dec );
    }
#undef TEST_TIGER_VAR
    vlc_mutex_unlock( &p_sys->lock );
    return VLC_SUCCESS;
}
