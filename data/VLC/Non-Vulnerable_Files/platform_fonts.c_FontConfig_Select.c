 ***/
char* FontConfig_Select( filter_t *p_filter, const char* family,
                          bool b_bold, bool b_italic, int i_size, int *i_idx )
{
    FcResult result = FcResultMatch;
    FcPattern *pat, *p_pat;
    FcChar8* val_s;
    FcBool val_b;
    char *ret = NULL;
    FcConfig* config = NULL;
    VLC_UNUSED(p_filter);
    /* Create a pattern and fills it */
    pat = FcPatternCreate();
    if (!pat) return NULL;
    /* */
    FcPatternAddString( pat, FC_FAMILY, (const FcChar8*)family );
    FcPatternAddBool( pat, FC_OUTLINE, FcTrue );
    FcPatternAddInteger( pat, FC_SLANT, b_italic ? FC_SLANT_ITALIC : FC_SLANT_ROMAN );
    FcPatternAddInteger( pat, FC_WEIGHT, b_bold ? FC_WEIGHT_EXTRABOLD : FC_WEIGHT_NORMAL );
    if( i_size > 0 )
    {
        FcPatternAddDouble( pat, FC_SIZE, (double)i_size );
    }
    /* */
    FcDefaultSubstitute( pat );
    if( !FcConfigSubstitute( config, pat, FcMatchPattern ) )
    {
        FcPatternDestroy( pat );
        return NULL;
    }
    /* Find the best font for the pattern, destroy the pattern */
    p_pat = FcFontMatch( config, pat, &result );
    FcPatternDestroy( pat );
    if( !p_pat || result == FcResultNoMatch ) return NULL;
    /* Check the new pattern */
    if( ( FcResultMatch != FcPatternGetBool( p_pat, FC_OUTLINE, 0, &val_b ) )
        || ( val_b != FcTrue ) )
    {
        FcPatternDestroy( p_pat );
        return NULL;
    }
    if( FcResultMatch != FcPatternGetInteger( p_pat, FC_INDEX, 0, i_idx ) )
    {
        *i_idx = 0;
    }
    if( FcResultMatch != FcPatternGetString( p_pat, FC_FAMILY, 0, &val_s ) )
    {
        FcPatternDestroy( p_pat );
        return NULL;
    }
    /* if( strcasecmp((const char*)val_s, family ) != 0 )
        msg_Warn( p_filter, "fontconfig: selected font family is not"
                            "the requested one: '%s' != '%s'\n",
                            (const char*)val_s, family );   */
    if( FcResultMatch == FcPatternGetString( p_pat, FC_FILE, 0, &val_s ) )
        ret = strdup( (const char*)val_s );
    FcPatternDestroy( p_pat );
    return ret;
}
