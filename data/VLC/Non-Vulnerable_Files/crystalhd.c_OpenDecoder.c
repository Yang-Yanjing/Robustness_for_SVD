*****************************************************************************/
static int OpenDecoder( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    /* Codec specifics */
    uint32_t i_bcm_codec_subtype = 0;
    switch ( p_dec->fmt_in.i_codec )
    {
    case VLC_CODEC_H264:
        if( p_dec->fmt_in.i_original_fourcc == VLC_FOURCC( 'a', 'v', 'c', '1' ) )
            i_bcm_codec_subtype = BC_MSUBTYPE_AVC1;
        else
            i_bcm_codec_subtype = BC_MSUBTYPE_H264;
        break;
    case VLC_CODEC_VC1:
        i_bcm_codec_subtype = BC_MSUBTYPE_VC1;
        break;
    case VLC_CODEC_WMV3:
        i_bcm_codec_subtype = BC_MSUBTYPE_WMV3;
        break;
    case VLC_CODEC_WMVA:
        i_bcm_codec_subtype = BC_MSUBTYPE_WMVA;
        break;
    case VLC_CODEC_MPGV:
        i_bcm_codec_subtype = BC_MSUBTYPE_MPEG2VIDEO;
        break;
/* Not ready for production yet
    case VLC_CODEC_MP4V:
        i_bcm_codec_subtype = BC_MSUBTYPE_DIVX;
        break;
    case VLC_CODEC_DIV3:
        i_bcm_codec_subtype = BC_MSUBTYPE_DIVX311;
        break; */
    default:
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store the decoder's structure */
    p_sys = malloc( sizeof(*p_sys) );
    if( !p_sys )
        return VLC_ENOMEM;
    /* Fill decoder_sys_t */
    p_dec->p_sys            = p_sys;
    p_sys->i_nal_size       = 4; // assume 4 byte start codes
    p_sys->i_sps_pps_size   = 0;
    p_sys->p_sps_pps_buf    = NULL;
    p_dec->p_sys->p_pic     = NULL;
    p_dec->p_sys->proc_out  = NULL;
    /* Win32 code *
     * We cannot link and ship BCM dll, even with LGPL license (too big)
     * and if we don't ship it, the plugin would not work depending on the
     * installation order => DLopen */
#ifdef USE_DL_OPENING
#  define DLL_NAME "bcmDIL.dll"
#  define PATHS_NB 3
    static const char *psz_paths[PATHS_NB] = {
    DLL_NAME,
    "C:\\Program Files\\Broadcom\\Broadcom CrystalHD Decoder\\" DLL_NAME,
    "C:\\Program Files (x86)\\Broadcom\\Broadcom CrystalHD Decoder\\" DLL_NAME,
    };
    for( int i = 0; i < PATHS_NB; i++ )
    {
        HINSTANCE p_bcm_dll = LoadLibraryA( psz_paths[i] );
        if( p_bcm_dll )
        {
            p_sys->p_bcm_dll = p_bcm_dll;
            break;
        }
    }
    if( !p_sys->p_bcm_dll )
    {
        msg_Dbg( p_dec, "Couldn't load the CrystalHD dll");
        return VLC_EGENERIC;
    }
#define LOAD_SYM( a ) \
    BC_FUNC( a )  = (void *)GetProcAddress( p_sys->p_bcm_dll, ( #a ) ); \
    if( !BC_FUNC( a ) ) { \
        msg_Err( p_dec, "missing symbol " # a ); return VLC_EGENERIC; }
#define LOAD_SYM_PSYS( a ) \
    p_sys->BC_FUNC( a )  = (void *)GetProcAddress( p_sys->p_bcm_dll, #a ); \
    if( !p_sys->BC_FUNC( a ) ) { \
        msg_Err( p_dec, "missing symbol " # a ); return VLC_EGENERIC; }
    BC_STATUS (WINAPI *OurDtsDeviceOpen)( HANDLE *hDevice, U32 mode );
    LOAD_SYM( DtsDeviceOpen );
    BC_STATUS (WINAPI *OurDtsCrystalHDVersion)( HANDLE  hDevice, PBC_INFO_CRYSTAL bCrystalInfo );
    LOAD_SYM( DtsCrystalHDVersion );
    BC_STATUS (WINAPI *OurDtsSetColorSpace)( HANDLE hDevice, BC_OUTPUT_FORMAT Mode422 );
    LOAD_SYM( DtsSetColorSpace );
    BC_STATUS (WINAPI *OurDtsSetInputFormat)( HANDLE hDevice, BC_INPUT_FORMAT *pInputFormat );
    LOAD_SYM( DtsSetInputFormat );
    BC_STATUS (WINAPI *OurDtsOpenDecoder)( HANDLE hDevice, U32 StreamType );
    LOAD_SYM( DtsOpenDecoder );
    BC_STATUS (WINAPI *OurDtsStartDecoder)( HANDLE hDevice );
    LOAD_SYM( DtsStartDecoder );
    BC_STATUS (WINAPI *OurDtsStartCapture)( HANDLE hDevice );
    LOAD_SYM( DtsStartCapture );
    LOAD_SYM_PSYS( DtsCloseDecoder );
    LOAD_SYM_PSYS( DtsDeviceClose );
    LOAD_SYM_PSYS( DtsFlushInput );
    LOAD_SYM_PSYS( DtsStopDecoder );
    LOAD_SYM_PSYS( DtsGetDriverStatus );
    LOAD_SYM_PSYS( DtsProcInput );
    LOAD_SYM_PSYS( DtsProcOutput );
    LOAD_SYM_PSYS( DtsIsEndOfStream );
#undef LOAD_SYM
#undef LOAD_SYM_PSYS
#endif /* USE_DL_OPENING */
#ifdef DEBUG_CRYSTALHD
    msg_Dbg( p_dec, "Trying to open CrystalHD HW");
#endif
    /* Get the handle for the device */
    if( BC_FUNC(DtsDeviceOpen)( &p_sys->bcm_handle,
             (DTS_PLAYBACK_MODE | DTS_LOAD_FILE_PLAY_FW | DTS_SKIP_TX_CHK_CPB) )
             // | DTS_DFLT_RESOLUTION(vdecRESOLUTION_720p29_97) ) )
             != BC_STS_SUCCESS )
    {
        msg_Err( p_dec, "Couldn't find and open the BCM CrystalHD device" );
        free( p_sys );
        return VLC_EGENERIC;
    }
#ifdef DEBUG_CRYSTALHD
    BC_INFO_CRYSTAL info;
    if( BC_FUNC(DtsCrystalHDVersion)( p_sys->bcm_handle, &info ) == BC_STS_SUCCESS )
    {
        msg_Dbg( p_dec, "Using CrystalHD Driver version: %i.%i.%i, "
            "Library version: %i.%i.%i, Firmware version: %i.%i.%i",
            info.drvVersion.drvRelease, info.drvVersion.drvMajor,
            info.drvVersion.drvMinor,
            info.dilVersion.dilRelease, info.dilVersion.dilMajor,
            info.dilVersion.dilMinor,
            info.fwVersion.fwRelease,   info.fwVersion.fwMajor,
            info.fwVersion.fwMinor );
    }
#endif
    /* Special case for AVC1 */
    if( i_bcm_codec_subtype == BC_MSUBTYPE_AVC1 )
    {
        if( p_dec->fmt_in.i_extra > 0 )
        {
            msg_Dbg( p_dec, "Parsing extra infos for avc1" );
            if( crystal_insert_sps_pps( p_dec, (uint8_t*)p_dec->fmt_in.p_extra,
                        p_dec->fmt_in.i_extra ) != VLC_SUCCESS )
                goto error;
        }
        else
        {
            msg_Err( p_dec, "Missing extra infos for avc1" );
            goto error;
        }
    }
    /* Always use YUY2 color */
    if( BC_FUNC(DtsSetColorSpace)( p_sys->bcm_handle, OUTPUT_MODE422_YUY2 )
            != BC_STS_SUCCESS )
    {
        msg_Err( p_dec, "Couldn't set the color space. Please report this!" );
        goto error;
    }
    /* Prepare Input for the device */
    BC_INPUT_FORMAT p_in;
    memset( &p_in, 0, sizeof(BC_INPUT_FORMAT) );
    p_in.OptFlags    = 0x51; /* 0b 0 1 01 0001 */
    p_in.mSubtype    = i_bcm_codec_subtype;
    p_in.startCodeSz = p_sys->i_nal_size;
    p_in.pMetaData   = p_sys->p_sps_pps_buf;
    p_in.metaDataSz  = p_sys->i_sps_pps_size;
    p_in.width       = p_dec->fmt_in.video.i_width;
    p_in.height      = p_dec->fmt_in.video.i_height;
    p_in.Progressive = true;
    if( BC_FUNC(DtsSetInputFormat)( p_sys->bcm_handle, &p_in ) != BC_STS_SUCCESS )
    {
        msg_Err( p_dec, "Couldn't set the color space. Please report this!" );
        goto error;
    }
    /* Open a decoder */
    if( BC_FUNC(DtsOpenDecoder)( p_sys->bcm_handle, BC_STREAM_TYPE_ES )
            != BC_STS_SUCCESS )
    {
        msg_Err( p_dec, "Couldn't open the CrystalHD decoder" );
        goto error;
    }
    /* Start it */
    if( BC_FUNC(DtsStartDecoder)( p_sys->bcm_handle ) != BC_STS_SUCCESS )
    {
        msg_Err( p_dec, "Couldn't start the decoder" );
        goto error;
    }
    if( BC_FUNC(DtsStartCapture)( p_sys->bcm_handle ) != BC_STS_SUCCESS )
    {
        msg_Err( p_dec, "Couldn't start the capture" );
        goto error_complete;
    }
    /* Set output properties */
    p_dec->fmt_out.i_cat          = VIDEO_ES;
    p_dec->fmt_out.i_codec        = VLC_CODEC_YUYV;
    p_dec->fmt_out.video.i_width  = p_dec->fmt_in.video.i_width;
    p_dec->fmt_out.video.i_height = p_dec->fmt_in.video.i_height;
    p_dec->b_need_packetized      = true;
    /* Set callbacks */
    p_dec->pf_decode_video = DecodeBlock;
    msg_Info( p_dec, "Opened CrystalHD hardware with success" );
    return VLC_SUCCESS;
error_complete:
    BC_FUNC_PSYS(DtsCloseDecoder)( p_sys->bcm_handle );
error:
    BC_FUNC_PSYS(DtsDeviceClose)( p_sys->bcm_handle );
    free( p_sys );
    return VLC_EGENERIC;
}
