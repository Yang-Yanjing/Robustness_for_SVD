#if !TARGET_OS_IPHONE
char* MacLegacy_Select( filter_t *p_filter, const char* psz_fontname,
                          bool b_bold, bool b_italic, int i_size, int *i_idx )
{
    VLC_UNUSED( b_bold );
    VLC_UNUSED( b_italic );
    VLC_UNUSED( i_size );
    FSRef ref;
    unsigned char path[MAXPATHLEN];
    char * psz_path;
    CFStringRef  cf_fontName;
    ATSFontRef   ats_font_id;
    *i_idx = 0;
    if( psz_fontname == NULL )
        return NULL;
    msg_Dbg( p_filter, "looking for %s", psz_fontname );
    cf_fontName = CFStringCreateWithCString( kCFAllocatorDefault, psz_fontname, kCFStringEncodingUTF8 );
    ats_font_id = ATSFontFindFromName( cf_fontName, kATSOptionFlagsIncludeDisabledMask );
    if ( ats_font_id == 0 || ats_font_id == 0xFFFFFFFFUL )
    {
        msg_Dbg( p_filter, "ATS couldn't find %s by name, checking family", psz_fontname );
        ats_font_id = ATSFontFamilyFindFromName( cf_fontName, kATSOptionFlagsDefault );
        if ( ats_font_id == 0 || ats_font_id == 0xFFFFFFFFUL )
        {
            msg_Dbg( p_filter, "ATS couldn't find either %s nor its family, checking PS name", psz_fontname );
            ats_font_id = ATSFontFindFromPostScriptName( cf_fontName, kATSOptionFlagsDefault );
            if ( ats_font_id == 0 || ats_font_id == 0xFFFFFFFFUL )
            {
                msg_Err( p_filter, "ATS couldn't find %s (no font name, family or PS name)", psz_fontname );
                CFRelease( cf_fontName );
                return NULL;
            }
        }
    }
    CFRelease( cf_fontName );
    if ( noErr != ATSFontGetFileReference( ats_font_id, &ref ) )
    {
        msg_Err( p_filter, "ATS couldn't get file ref for %s", psz_fontname );
        return NULL;
    }
    /* i_idx calculation by searching preceding fontIDs */
    /* with same FSRef                                       */
    {
        ATSFontRef  id2 = ats_font_id - 1;
        FSRef       ref2;
        while ( id2 > 0 )
        {
            if ( noErr != ATSFontGetFileReference( id2, &ref2 ) )
                break;
            if ( noErr != FSCompareFSRefs( &ref, &ref2 ) )
                break;
            id2 --;
        }
        *i_idx = ats_font_id - ( id2 + 1 );
    }
    if ( noErr != FSRefMakePath( &ref, path, sizeof(path) ) )
    {
        msg_Err( p_filter, "failure when getting path from FSRef" );
        return NULL;
    }
    msg_Dbg( p_filter, "found %s", path );
    psz_path = strdup( (char *)path );
    return psz_path;
}
