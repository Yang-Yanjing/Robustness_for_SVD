 *****************************************************************************/
static int Open(vlc_object_t *p_this)
{
    intf_thread_t *intf = (intf_thread_t *)p_this;
    intf_sys_t    *sys  = intf->p_sys = calloc(1, sizeof(intf_sys_t));
    playlist_t    *p_playlist = pl_Get(intf);
    if (!sys)
        return VLC_ENOMEM;
    vlc_mutex_init(&sys->msg_lock);
    vlc_mutex_init(&sys->pl_lock);
    sys->verbosity = var_InheritInteger(intf, "verbose");
    vlc_LogSet(intf->p_libvlc, MsgCallback, sys);
    sys->box_type = BOX_PLAYLIST;
    sys->plidx_follow = true;
    sys->color = var_CreateGetBool(intf, "color");
    sys->current_dir = var_CreateGetNonEmptyString(intf, "browse-dir");
    if (!sys->current_dir)
        sys->current_dir = config_GetUserDir(VLC_HOME_DIR);
    initscr();   /* Initialize the curses library */
    if (sys->color)
        start_color_and_pairs(intf);
    keypad(stdscr, TRUE);
    nonl();                 /* Don't do NL -> CR/NL */
    cbreak();               /* Take input chars one at a time */
    noecho();               /* Don't echo */
    curs_set(0);            /* Invisible cursor */
    timeout(1000);          /* blocking getch() */
    clear();
    /* Stop printing errors to the console */
    if (!freopen("/dev/null", "wb", stderr))
        msg_Err(intf, "Couldn't close stderr (%s)", vlc_strerror_c(errno));
    ReadDir(intf);
    PL_LOCK;
    PlaylistRebuild(intf),
    PL_UNLOCK;
    if (vlc_clone(&sys->thread, Run, intf, VLC_THREAD_PRIORITY_LOW))
        abort(); /* TODO */
    return VLC_SUCCESS;
}
