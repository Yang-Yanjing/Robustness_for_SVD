 *****************************************************************************/
static int QTAudioInit( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
#ifdef __APPLE__
    p_sys->SoundConverterOpen       = (void*)SoundConverterOpen;
    p_sys->SoundConverterClose      = (void*)SoundConverterClose;
    p_sys->SoundConverterSetInfo    = (void*)SoundConverterSetInfo;
    p_sys->SoundConverterGetBufferSizes = (void*)SoundConverterGetBufferSizes;
    p_sys->SoundConverterConvertBuffer  = (void*)SoundConverterConvertBuffer;
    p_sys->SoundConverterBeginConversion= (void*)SoundConverterBeginConversion;
    p_sys->SoundConverterEndConversion  = (void*)SoundConverterEndConversion;
#else
#ifdef LOADER
    p_sys->ldt_fs = Setup_LDT_Keeper();
#endif /* LOADER */
    p_sys->qts = LoadLibraryA( "QuickTime.qts" );
    if( p_sys->qts == NULL )
    {
        msg_Dbg( p_dec, "failed loading QuickTime.qts" );
        return VLC_EGENERIC;
    }
    p_sys->qtml = LoadLibraryA( "qtmlClient.dll" );
    if( p_sys->qtml == NULL )
    {
        msg_Dbg( p_dec, "failed loading qtmlClient.dll" );
        return VLC_EGENERIC;
    }
    p_sys->InitializeQTML               = (void *)GetProcAddress( p_sys->qtml, "InitializeQTML" );
    p_sys->TerminateQTML                = (void *)GetProcAddress( p_sys->qtml, "TerminateQTML" );
    p_sys->SoundConverterOpen           = (void *)GetProcAddress( p_sys->qtml, "SoundConverterOpen" );
    p_sys->SoundConverterClose          = (void *)GetProcAddress( p_sys->qtml, "SoundConverterClose" );
    p_sys->SoundConverterSetInfo        = (void *)GetProcAddress( p_sys->qtml, "SoundConverterSetInfo" );
    p_sys->SoundConverterGetBufferSizes = (void *)GetProcAddress( p_sys->qtml, "SoundConverterGetBufferSizes" );
    p_sys->SoundConverterConvertBuffer  = (void *)GetProcAddress( p_sys->qtml, "SoundConverterConvertBuffer" );
    p_sys->SoundConverterEndConversion  = (void *)GetProcAddress( p_sys->qtml, "SoundConverterEndConversion" );
    p_sys->SoundConverterBeginConversion= (void *)GetProcAddress( p_sys->qtml, "SoundConverterBeginConversion");
    if( p_sys->InitializeQTML == NULL )
    {
        msg_Err( p_dec, "failed getting proc address InitializeQTML" );
        return VLC_EGENERIC;
    }
    if( p_sys->SoundConverterOpen == NULL ||
        p_sys->SoundConverterClose == NULL ||
        p_sys->SoundConverterSetInfo == NULL ||
        p_sys->SoundConverterGetBufferSizes == NULL ||
        p_sys->SoundConverterConvertBuffer == NULL ||
        p_sys->SoundConverterEndConversion == NULL ||
        p_sys->SoundConverterBeginConversion == NULL )
    {
        msg_Err( p_dec, "failed getting proc address" );
        return VLC_EGENERIC;
    }
    msg_Dbg( p_dec, "standard init done" );
#endif /* else __APPLE__ */
    return VLC_SUCCESS;
}
