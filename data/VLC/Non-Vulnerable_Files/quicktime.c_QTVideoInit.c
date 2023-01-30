 *****************************************************************************/
static int QTVideoInit( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
#ifdef __APPLE__
    p_sys->FindNextComponent        = (void*)FindNextComponent;
    p_sys->OpenComponent            = (void*)OpenComponent;
    p_sys->ImageCodecInitialize     = (void*)ImageCodecInitialize;
    p_sys->ImageCodecGetCodecInfo   = (void*)ImageCodecGetCodecInfo;
    p_sys->ImageCodecPreDecompress  = (void*)ImageCodecPreDecompress;
    p_sys->ImageCodecBandDecompress = (void*)ImageCodecBandDecompress;
    p_sys->GetGWorldPixMap          = (void*)GetGWorldPixMap;
    p_sys->QTNewGWorldFromPtr       = (void*)QTNewGWorldFromPtr;
    p_sys->NewHandleClear           = (void*)NewHandleClear;
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
    msg_Dbg( p_dec, "QuickTime.qts loaded" );
    p_sys->qtml = LoadLibraryA( "qtmlClient.dll" );
    if( p_sys->qtml == NULL )
    {
        msg_Dbg( p_dec, "failed loading qtmlClient.dll" );
        return VLC_EGENERIC;
    }
    msg_Dbg( p_dec, "qtmlClient.dll loaded" );
    /* (void*) to shut up gcc */
    p_sys->InitializeQTML           = (void*)GetProcAddress( p_sys->qtml, "InitializeQTML" );
    p_sys->FindNextComponent        = (void*)GetProcAddress( p_sys->qtml, "FindNextComponent" );
    p_sys->OpenComponent            = (void*)GetProcAddress( p_sys->qtml, "OpenComponent" );
    p_sys->ImageCodecInitialize     = (void*)GetProcAddress( p_sys->qtml, "ImageCodecInitialize" );
    p_sys->ImageCodecGetCodecInfo   = (void*)GetProcAddress( p_sys->qtml, "ImageCodecGetCodecInfo" );
    p_sys->ImageCodecPreDecompress  = (void*)GetProcAddress( p_sys->qtml, "ImageCodecPreDecompress" );
    p_sys->ImageCodecBandDecompress = (void*)GetProcAddress( p_sys->qtml, "ImageCodecBandDecompress" );
    p_sys->GetGWorldPixMap          = (void*)GetProcAddress( p_sys->qtml, "GetGWorldPixMap" );
    p_sys->QTNewGWorldFromPtr       = (void*)GetProcAddress( p_sys->qtml, "QTNewGWorldFromPtr" );
    p_sys->NewHandleClear           = (void*)GetProcAddress( p_sys->qtml, "NewHandleClear" );
    if( p_sys->InitializeQTML == NULL )
    {
        msg_Dbg( p_dec, "failed getting proc address InitializeQTML" );
        return VLC_EGENERIC;
    }
    if( p_sys->FindNextComponent == NULL ||
        p_sys->OpenComponent == NULL ||
        p_sys->ImageCodecInitialize == NULL ||
        p_sys->ImageCodecGetCodecInfo == NULL ||
        p_sys->ImageCodecPreDecompress == NULL ||
        p_sys->ImageCodecBandDecompress == NULL ||
        p_sys->GetGWorldPixMap == NULL ||
        p_sys->QTNewGWorldFromPtr == NULL ||
        p_sys->NewHandleClear == NULL )
    {
        msg_Err( p_dec, "failed getting proc address" );
        return VLC_EGENERIC;
    }
#endif /* __APPLE__ */
    return VLC_SUCCESS;
}
