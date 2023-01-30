}
static void FillBox(intf_thread_t *intf)
{
    intf_sys_t *sys = intf->p_sys;
    static int (* const draw[]) (intf_thread_t *) = {
        [BOX_HELP]      = DrawHelp,
        [BOX_INFO]      = DrawInfo,
        [BOX_META]      = DrawMeta,
        [BOX_OBJECTS]   = DrawObjects,
        [BOX_STATS]     = DrawStats,
        [BOX_BROWSE]    = DrawBrowse,
        [BOX_PLAYLIST]  = DrawPlaylist,
        [BOX_SEARCH]    = DrawPlaylist,
        [BOX_OPEN]      = DrawPlaylist,
        [BOX_LOG]       = DrawMessages,
    };
    sys->box_lines_total = draw[sys->box_type](intf);
    if (sys->box_type == BOX_SEARCH || sys->box_type == BOX_OPEN)
        FillTextBox(sys);
}
