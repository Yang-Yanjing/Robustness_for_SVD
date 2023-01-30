#ifdef HAVE_FONTCONFIG
void FontConfig_BuildCache( filter_t *p_filter )
{
    /* */
    msg_Dbg( p_filter, "Building font databases.");
    mtime_t t1, t2;
    t1 = mdate();
#ifdef __OS2__
    FcInit();
#endif
#if defined( _WIN32 ) || defined( __APPLE__ )
    dialog_progress_bar_t *p_dialog = NULL;
    FcConfig *fcConfig = FcInitLoadConfig();
    p_dialog = dialog_ProgressCreate( p_filter,
            _("Building font cache"),
            _("Please wait while your font cache is rebuilt.\n"
                "This should take less than a few minutes."), NULL );
/*    if( p_dialog )
        dialog_ProgressSet( p_dialog, NULL, 0.5 ); */
    FcConfigBuildFonts( fcConfig );
#if defined( __APPLE__ )
    // By default, scan only the directory /System/Library/Fonts.
    // So build the set of available fonts under another directories,
    // and add the set to the current configuration.
    FcConfigAppFontAddDir( NULL, "~/Library/Fonts" );
    FcConfigAppFontAddDir( NULL, "/Library/Fonts" );
    FcConfigAppFontAddDir( NULL, "/Network/Library/Fonts" );
    //FcConfigAppFontAddDir( NULL, "/System/Library/Fonts" );
#endif
    if( p_dialog )
    {
//        dialog_ProgressSet( p_dialog, NULL, 1.0 );
        dialog_ProgressDestroy( p_dialog );
        p_dialog = NULL;
    }
#endif
    t2 = mdate();
    msg_Dbg( p_filter, "Took %ld microseconds", (long)((t2 - t1)) );
}
