 *****************************************************************************/
static int OpenVideo( decoder_t *p_dec )
{
#ifndef _WIN32
    decoder_sys_t *p_sys = malloc( sizeof( decoder_sys_t ) );
    if( !p_sys )
        return VLC_ENOMEM;
    long                                i_result;
    ComponentDescription                desc;
    Component                           prev;
    ComponentResult                     cres;
    ImageSubCodecDecompressCapabilities icap;   /* for ImageCodecInitialize() */
    CodecInfo                           cinfo;  /* for ImageCodecGetCodecInfo() */
    ImageDescription                    *id;
    char                fcc[4];
    int     i_vide = p_dec->fmt_in.i_extra;
    uint8_t *p_vide = p_dec->fmt_in.p_extra;
    p_dec->p_sys = p_sys;
    p_dec->pf_decode_video = DecodeVideo;
    p_sys->i_late = 0;
    if( i_vide <= 0 )
    {
        msg_Err( p_dec, "missing extra info" );
        free( p_sys );
        return VLC_EGENERIC;
    }
    if( p_dec->fmt_in.i_original_fourcc )
        memcpy( fcc, &p_dec->fmt_in.i_original_fourcc, 4 );
    else
        memcpy( fcc, &p_dec->fmt_in.i_codec, 4 );
    msg_Dbg( p_dec, "quicktime_video %4.4s %dx%d",
             fcc, p_dec->fmt_in.video.i_width, p_dec->fmt_in.video.i_height );
    /* get lock, avoid segfault */
    vlc_mutex_lock( &qt_mutex );
#ifdef __APPLE__
    EnterMovies();
#endif
    if( QTVideoInit( p_dec ) )
    {
        msg_Err( p_dec, "cannot initialize QT");
        goto exit_error;
    }
#ifndef __APPLE__
    if( ( i_result = p_sys->InitializeQTML( 6 + 16 ) ) )
    {
        msg_Dbg( p_dec, "error on InitializeQTML = %d", (int)i_result );
        goto exit_error;
    }
#endif
    /* init ComponentDescription */
    memset( &desc, 0, sizeof( ComponentDescription ) );
    desc.componentType      = FCC( 'i', 'm', 'd', 'c' );
    desc.componentSubType   = FCC( fcc[0], fcc[1], fcc[2], fcc[3] );
    desc.componentManufacturer = 0;
    desc.componentFlags        = 0;
    desc.componentFlagsMask    = 0;
    if( !( prev = p_sys->FindNextComponent( NULL, &desc ) ) )
    {
        msg_Err( p_dec, "cannot find requested component" );
        goto exit_error;
    }
    msg_Dbg( p_dec, "component id=0x%p", prev );
    p_sys->ci =  p_sys->OpenComponent( prev );
    msg_Dbg( p_dec, "component instance p=0x%p", p_sys->ci );
    memset( &icap, 0, sizeof( ImageSubCodecDecompressCapabilities ) );
    cres =  p_sys->ImageCodecInitialize( p_sys->ci, &icap );
    msg_Dbg( p_dec, "ImageCodecInitialize->0x%X size=%d (%d)",
             (int)cres, (int)icap.recordSize, (int)icap.decompressRecordSize);
    memset( &cinfo, 0, sizeof( CodecInfo ) );
    cres =  p_sys->ImageCodecGetCodecInfo( p_sys->ci, &cinfo );
    msg_Dbg( p_dec,
             "Flags: compr: 0x%x decomp: 0x%x format: 0x%x",
             (unsigned int)cinfo.compressFlags,
             (unsigned int)cinfo.decompressFlags,
             (unsigned int)cinfo.formatFlags );
    msg_Dbg( p_dec, "quicktime_video: Codec name: %.*s",
             ((unsigned char*)&cinfo.typeName)[0],
             ((unsigned char*)&cinfo.typeName)+1 );
    /* make a yuy2 gworld */
    p_sys->OutBufferRect.top    = 0;
    p_sys->OutBufferRect.left   = 0;
    p_sys->OutBufferRect.right  = p_dec->fmt_in.video.i_width;
    p_sys->OutBufferRect.bottom = p_dec->fmt_in.video.i_height;
    /* codec data FIXME use codec not SVQ3 */
    msg_Dbg( p_dec, "vide = %d", i_vide  );
    id = malloc( sizeof( ImageDescription ) + ( i_vide - 70 ) );
    if( !id )
        goto exit_error;
    id->idSize          = sizeof( ImageDescription ) + ( i_vide - 70 );
    id->cType           = FCC( fcc[0], fcc[1], fcc[2], fcc[3] );
    id->version         = GetWBE ( p_vide +  0 );
    id->revisionLevel   = GetWBE ( p_vide +  2 );
    id->vendor          = GetDWBE( p_vide +  4 );
    id->temporalQuality = GetDWBE( p_vide +  8 );
    id->spatialQuality  = GetDWBE( p_vide + 12 );
    id->width           = GetWBE ( p_vide + 16 );
    id->height          = GetWBE ( p_vide + 18 );
    id->hRes            = GetDWBE( p_vide + 20 );
    id->vRes            = GetDWBE( p_vide + 24 );
    id->dataSize        = GetDWBE( p_vide + 28 );
    id->frameCount      = GetWBE ( p_vide + 32 );
    memcpy( &id->name, p_vide + 34, 32 );
    id->depth           = GetWBE ( p_vide + 66 );
    id->clutID          = GetWBE ( p_vide + 68 );
    if( i_vide > 70 )
    {
        memcpy( ((char*)&id->clutID) + 2, p_vide + 70, i_vide - 70 );
    }
    msg_Dbg( p_dec, "idSize=%d ver=%d rev=%d vendor=%d tempQ=%d "
             "spaQ=%d w=%d h=%d dpi=%d%d dataSize=%d depth=%d frameCount=%d clutID=%d",
             (int)id->idSize, id->version, id->revisionLevel, (int)id->vendor,
             (int)id->temporalQuality, (int)id->spatialQuality,
             (int)id->width, (int)id->height,
             (int)id->hRes, (int)id->vRes,
             (int)id->dataSize,
             id->depth,
             id->frameCount,
             id->clutID );
    p_sys->framedescHandle = (ImageDescriptionHandle) NewHandleClear( id->idSize );
    memcpy( *p_sys->framedescHandle, id, id->idSize );
    if( p_dec->fmt_in.video.i_width != 0 && p_dec->fmt_in.video.i_height != 0) 
        p_sys->plane = malloc( p_dec->fmt_in.video.i_width * p_dec->fmt_in.video.i_height * 3 );
    if( !p_sys->plane )
        goto exit_error;
    i_result = p_sys->QTNewGWorldFromPtr( &p_sys->OutBufferGWorld,
                                          /*pixel format of new GWorld==YUY2 */
                                          kYUVSPixelFormat,
                                          /* we should benchmark if yvu9 is
                                           * faster for svq3, too */
                                          &p_sys->OutBufferRect,
                                          0, 0, 0,
                                          p_sys->plane,
                                          p_dec->fmt_in.video.i_width * 2 );
    msg_Dbg( p_dec, "NewGWorldFromPtr returned:%ld",
             65536 - ( i_result&0xffff ) );
    memset( &p_sys->decpar, 0, sizeof( CodecDecompressParams ) );
    p_sys->decpar.imageDescription = p_sys->framedescHandle;
    p_sys->decpar.startLine        = 0;
    p_sys->decpar.stopLine         = ( **p_sys->framedescHandle ).height;
    p_sys->decpar.frameNumber      = 1;
    p_sys->decpar.matrixFlags      = 0;
    p_sys->decpar.matrixType       = 0;
    p_sys->decpar.matrix           = 0;
    p_sys->decpar.capabilities     = &p_sys->codeccap;
    p_sys->decpar.accuracy         = codecNormalQuality;
    p_sys->decpar.srcRect          = p_sys->OutBufferRect;
    p_sys->decpar.transferMode     = srcCopy;
    p_sys->decpar.dstPixMap        = **p_sys->GetGWorldPixMap( p_sys->OutBufferGWorld );/*destPixmap;  */
    cres =  p_sys->ImageCodecPreDecompress( p_sys->ci, &p_sys->decpar );
    msg_Dbg( p_dec, "quicktime_video: ImageCodecPreDecompress cres=0x%X",
             (int)cres );
    es_format_Init( &p_dec->fmt_out, VIDEO_ES, VLC_CODEC_YUYV);
    p_dec->fmt_out.video.i_width = p_dec->fmt_in.video.i_width;
    p_dec->fmt_out.video.i_height= p_dec->fmt_in.video.i_height;
    p_dec->fmt_out.video.i_sar_num = 1;
    p_dec->fmt_out.video.i_sar_den = 1;
    vlc_mutex_unlock( &qt_mutex );
    return VLC_SUCCESS;
exit_error:
#ifdef LOADER
    Restore_LDT_Keeper( p_sys->ldt_fs );
#endif
    vlc_mutex_unlock( &qt_mutex );
#else
    VLC_UNUSED( p_dec );
#endif /* !_WIN32 */
    return VLC_EGENERIC;
}
