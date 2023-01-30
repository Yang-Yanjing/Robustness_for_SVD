void
VCDUpdateTitle( access_t *p_access )
{
    vcdplayer_t *p_vcdplayer= (vcdplayer_t *)p_access->p_sys;
    size_t psz_mrl_max = strlen(VCD_MRL_PREFIX)
                       + strlen(p_vcdplayer->psz_source) + sizeof("@E999")+3;
    char *psz_mrl = malloc( psz_mrl_max );
    if( psz_mrl )
    {
        char *psz_name;
        char *psz_tfmt = var_InheritString( p_access, MODULE_STRING "-title-format" );
        snprintf( psz_mrl, psz_mrl_max, "%s%s",
                  VCD_MRL_PREFIX, p_vcdplayer->psz_source );
        if( psz_tfmt )
        {
            psz_name = VCDFormatStr( p_vcdplayer, psz_tfmt, psz_mrl,
                                     &(p_vcdplayer->play_item) );
            free(psz_tfmt);
            input_Control( p_vcdplayer->p_input, INPUT_SET_NAME, psz_name );
            free(psz_name);
        }
        free(psz_mrl);
    }
}
