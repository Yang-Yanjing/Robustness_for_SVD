                                          "VMAN"};
static void PMThread( void *arg )
{
    vout_display_t *vd = ( vout_display_t * )arg;
    vout_display_sys_t * sys = vd->sys;
    ULONG i_frame_flags;
    QMSG qm;
    char *psz_mode;
    ULONG i_kva_mode;
    /* */
    video_format_t fmt;
    video_format_ApplyRotation(&fmt, &vd->fmt);
    /* */
    vout_display_info_t info = vd->info;
    info.is_slow = false;
    info.has_double_click = true;
    info.has_hide_mouse = false;
    info.has_pictures_invalid = false;
    MorphToPM();
    sys->hab = WinInitialize( 0 );
    sys->hmq = WinCreateMsgQueue( sys->hab, 0);
    WinRegisterClass( sys->hab,
                      WC_VLC_KVA,
                      WndProc,
                      CS_SIZEREDRAW | CS_MOVENOTIFY,
                      sizeof( PVOID ));
    sys->b_fixt23 = var_CreateGetBool( vd, "kva-fixt23");
    if( !sys->b_fixt23 )
    {
        vout_window_cfg_t wnd_cfg;
        wnd_cfg.is_standalone = false;
        wnd_cfg.type          = VOUT_WINDOW_TYPE_HWND;
        wnd_cfg.x             = var_InheritInteger(vd, "video-x");
        wnd_cfg.y             = var_InheritInteger(vd, "video-y");
        wnd_cfg.width         = vd->cfg->display.width;
        wnd_cfg.height        = vd->cfg->display.height;
        /* If an external window was specified, we'll draw in it. */
        sys->parent_window =
            vout_display_NewWindow( vd, &wnd_cfg );
    }
    if( sys->parent_window )
    {
        sys->parent = ( HWND )sys->parent_window->handle.hwnd;
        ULONG i_style = WinQueryWindowULong( sys->parent, QWL_STYLE );
        WinSetWindowULong( sys->parent, QWL_STYLE,
                           i_style | WS_CLIPCHILDREN );
        i_frame_flags = FCF_TITLEBAR;
    }
    else
    {
        sys->parent = HWND_DESKTOP;
        i_frame_flags = FCF_SYSMENU    | FCF_TITLEBAR | FCF_MINMAX |
                        FCF_SIZEBORDER | FCF_TASKLIST;
    }
    sys->frame =
        WinCreateStdWindow( sys->parent,      /* parent window handle */
                            WS_VISIBLE,       /* frame window style */
                            &i_frame_flags,   /* window style */
                            WC_VLC_KVA,       /* class name */
                            "",               /* window title */
                            0L,               /* default client style */
                            NULLHANDLE,       /* resource in exe file */
                            1,                /* frame window id */
                            &sys->client );   /* client window handle */
    if( sys->frame == NULLHANDLE )
    {
        msg_Err( vd, "cannot create a frame window");
        goto exit_frame;
    }
    WinSetWindowPtr( sys->client, 0, vd );
    if( !sys->parent_window )
    {
        WinSetWindowPtr( sys->frame, 0, vd );
        sys->p_old_frame = WinSubclassWindow( sys->frame, MyFrameWndProc );
    }
    psz_mode = var_CreateGetString( vd, "kva-video-mode" );
    i_kva_mode = KVAM_AUTO;
    if( strcmp( psz_mode, "snap" ) == 0 )
        i_kva_mode = KVAM_SNAP;
    else if( strcmp( psz_mode, "wo" ) == 0 )
        i_kva_mode = KVAM_WO;
    else if( strcmp( psz_mode, "vman" ) == 0 )
        i_kva_mode = KVAM_VMAN;
    else if( strcmp( psz_mode, "dive" ) == 0 )
        i_kva_mode = KVAM_DIVE;
    free( psz_mode );
    if( kvaInit( i_kva_mode, sys->client, COLOR_KEY ))
    {
        msg_Err( vd, "cannot initialize KVA");
        goto exit_kva_init;
    }
    kvaCaps( &sys->kvac );
    msg_Dbg( vd, "selected video mode = %s",
             psz_video_mode[ sys->kvac.ulMode - 1 ]);
    if( OpenDisplay( vd, &fmt ) )
    {
        msg_Err( vd, "cannot open display");
        goto exit_open_display;
    }
    if( vd->cfg->is_fullscreen )
    {
        if( sys->parent_window )
            vout_window_SetFullScreen(sys->parent_window, true);
        else
            WinPostMsg( sys->client, WM_VLC_FULLSCREEN_CHANGE,
                        MPFROMLONG( true ), 0 );
    }
    kvaDisableScreenSaver();
    /* Setup vout_display now that everything is fine */
    vd->fmt     = fmt;
    vd->info    = info;
    vd->pool    = Pool;
    vd->prepare = NULL;
    vd->display = Display;
    vd->control = Control;
    vd->manage  = Manage;
    /* Prevent SIG_FPE */
    _control87(MCW_EM, MCW_EM);
    sys->i_result = VLC_SUCCESS;
    DosPostEventSem( sys->ack_event );
    if( !sys->parent_window )
        WinSetVisibleRegionNotify( sys->frame, TRUE );
    while( WinGetMsg( sys->hab, &qm, NULLHANDLE, 0, 0 ))
        WinDispatchMsg( sys->hab, &qm );
    if( !sys->parent_window )
        WinSetVisibleRegionNotify( sys->frame, FALSE );
    kvaEnableScreenSaver();
    CloseDisplay( vd );
    /* fall through */
exit_open_display :
    kvaDone();
exit_kva_init :
    if( !sys->parent_window )
        WinSubclassWindow( sys->frame, sys->p_old_frame );
    WinDestroyWindow( sys->frame );
exit_frame :
    vout_display_DeleteWindow( vd, sys->parent_window );
    if( sys->is_mouse_hidden )
        WinShowPointer( HWND_DESKTOP, TRUE );
    WinDestroyMsgQueue( sys->hmq );
    WinTerminate( sys->hab );
    sys->i_result = VLC_EGENERIC;
    DosPostEventSem( sys->ack_event );
}
