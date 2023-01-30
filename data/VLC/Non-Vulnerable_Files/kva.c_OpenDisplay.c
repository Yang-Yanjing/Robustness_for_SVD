 *****************************************************************************/
static int OpenDisplay( vout_display_t *vd, video_format_t *fmt )
{
    vout_display_sys_t * sys = vd->sys;
    const vlc_fourcc_t *fallback;
    bool b_hw_accel = 0;
    FOURCC i_kva_fourcc;
    int i_chroma_shift;
    char sz_title[ 256 ];
    RECTL rcl;
    int w, h;
    msg_Dbg( vd, "render chroma = %4.4s", ( const char * )&fmt->i_chroma );
    for( int pass = 0; pass < 2 && !b_hw_accel; pass++ )
    {
        fallback = ( pass == 0 ) ? vlc_fourcc_GetYUVFallback( fmt->i_chroma ) :
                                   vlc_fourcc_GetRGBFallback( fmt->i_chroma );
        for( int i = 0; fallback[ i ]; i++ )
        {
            switch( fallback[ i ])
            {
                case VLC_CODEC_YV12:
                    b_hw_accel = sys->kvac.ulInputFormatFlags & KVAF_YV12;
                    i_kva_fourcc = FOURCC_YV12;
                    i_chroma_shift = 1;
                    break;
                case VLC_CODEC_YUYV:
                    b_hw_accel = sys->kvac.ulInputFormatFlags & KVAF_YUY2;
                    i_kva_fourcc = FOURCC_Y422;
                    i_chroma_shift = 0;
                    break;
                case VLC_CODEC_YV9:
                    b_hw_accel = sys->kvac.ulInputFormatFlags & KVAF_YVU9;
                    i_kva_fourcc = FOURCC_YVU9;
                    i_chroma_shift = 2;
                    break;
                case VLC_CODEC_RGB32:
                    b_hw_accel = sys->kvac.ulInputFormatFlags & KVAF_BGR32;
                    i_kva_fourcc = FOURCC_BGR4;
                    i_chroma_shift = 0;
                    break;
                case VLC_CODEC_RGB24:
                    b_hw_accel = sys->kvac.ulInputFormatFlags & KVAF_BGR24;
                    i_kva_fourcc = FOURCC_BGR3;
                    i_chroma_shift = 0;
                    break;
                case VLC_CODEC_RGB16:
                    b_hw_accel = sys->kvac.ulInputFormatFlags & KVAF_BGR16;
                    i_kva_fourcc = FOURCC_R565;
                    i_chroma_shift = 0;
                    break;
                case VLC_CODEC_RGB15:
                    b_hw_accel = sys->kvac.ulInputFormatFlags & KVAF_BGR15;
                    i_kva_fourcc = FOURCC_R555;
                    i_chroma_shift = 0;
                    break;
            }
            if( b_hw_accel )
            {
                fmt->i_chroma = fallback[ i ];
                break;
            }
        }
    }
    if( !b_hw_accel )
    {
        msg_Err( vd, "Ooops. There is no fourcc supported by KVA at all.");
        return VLC_EGENERIC;
    }
    /* Set the RGB masks */
    fmt->i_rmask = sys->kvac.ulRMask;
    fmt->i_gmask = sys->kvac.ulGMask;
    fmt->i_bmask = sys->kvac.ulBMask;
    msg_Dbg( vd, "output chroma = %4.4s", ( const char * )&fmt->i_chroma );
    msg_Dbg( vd, "KVA chroma = %4.4s", ( const char * )&i_kva_fourcc );
    w = fmt->i_width;
    h = fmt->i_height;
    sys->kvas.ulLength           = sizeof( KVASETUP );
    sys->kvas.szlSrcSize.cx      = w;
    sys->kvas.szlSrcSize.cy      = h;
    sys->kvas.rclSrcRect.xLeft   = fmt->i_x_offset;
    sys->kvas.rclSrcRect.yTop    = fmt->i_y_offset;
    sys->kvas.rclSrcRect.xRight  = fmt->i_x_offset + fmt->i_visible_width;
    sys->kvas.rclSrcRect.yBottom = fmt->i_y_offset + fmt->i_visible_height;
    sys->kvas.ulRatio            = KVAR_FORCEANY;
    sys->kvas.ulAspectWidth      = w;
    sys->kvas.ulAspectHeight     = h;
    sys->kvas.fccSrcColor        = i_kva_fourcc;
    sys->kvas.fDither            = TRUE;
    if( kvaSetup( &sys->kvas ))
    {
        msg_Err( vd, "cannot set up KVA");
        return VLC_EGENERIC;
    }
    /* Create the associated picture */
    picture_sys_t *picsys = malloc( sizeof( *picsys ) );
    if( picsys == NULL )
        return VLC_ENOMEM;
    picsys->i_chroma_shift = i_chroma_shift;
    picture_resource_t resource = { .p_sys = picsys };
    picture_t *picture = picture_NewFromResource( fmt, &resource );
    if( !picture )
    {
        free( picsys );
        return VLC_ENOMEM;
    }
    /* Wrap it into a picture pool */
    picture_pool_configuration_t pool_cfg;
    memset( &pool_cfg, 0, sizeof( pool_cfg ));
    pool_cfg.picture_count = 1;
    pool_cfg.picture       = &picture;
    pool_cfg.lock          = KVALock;
    pool_cfg.unlock        = KVAUnlock;
    sys->pool = picture_pool_NewExtended( &pool_cfg );
    if( !sys->pool )
    {
        picture_Release( picture );
        return VLC_ENOMEM;
    }
    if (vd->cfg->display.title)
        snprintf( sz_title, sizeof( sz_title ), "%s", vd->cfg->display.title );
    else
        snprintf( sz_title, sizeof( sz_title ),
                  "%s (%4.4s to %4.4s - %s mode KVA output)",
                  VOUT_TITLE,
                  ( char * )&vd->fmt.i_chroma,
                  ( char * )&sys->kvas.fccSrcColor,
                  psz_video_mode[ sys->kvac.ulMode - 1 ]);
    WinSetWindowText( sys->frame, sz_title );
    sys->i_screen_width  = WinQuerySysValue( HWND_DESKTOP, SV_CXSCREEN );
    sys->i_screen_height = WinQuerySysValue( HWND_DESKTOP, SV_CYSCREEN );
    if( sys->parent_window )
        WinQueryWindowRect( sys->parent, &sys->client_rect );
    else
    {
        sys->client_rect.xLeft   = ( sys->i_screen_width  - w ) / 2;
        sys->client_rect.yBottom = ( sys->i_screen_height - h ) / 2 ;
        sys->client_rect.xRight  = sys->client_rect.xLeft   + w;
        sys->client_rect.yTop    = sys->client_rect.yBottom + h;
    }
    rcl = sys->client_rect;
    WinCalcFrameRect( sys->frame, &rcl, FALSE);
    WinSetWindowPos( sys->frame, HWND_TOP,
                     rcl.xLeft, rcl.yBottom,
                     rcl.xRight - rcl.xLeft, rcl.yTop - rcl.yBottom,
                     SWP_MOVE | SWP_SIZE | SWP_ZORDER | SWP_SHOW |
                     SWP_ACTIVATE );
    return VLC_SUCCESS;
}
