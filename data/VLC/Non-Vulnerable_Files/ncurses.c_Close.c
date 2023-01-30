 *****************************************************************************/
static void Close(vlc_object_t *p_this)
{
    intf_sys_t *sys = ((intf_thread_t*)p_this)->p_sys;
    vlc_join(sys->thread, NULL);
    PlaylistDestroy(sys);
    DirsDestroy(sys);
    free(sys->current_dir);
    if (sys->p_input)
        vlc_object_release(sys->p_input);
    if (can_change_color())
        /* Restore yellow to its original color */
        init_color(COLOR_YELLOW, sys->yellow_r, sys->yellow_g, sys->yellow_b);
    endwin();   /* Close the ncurses interface */
    vlc_LogSet(p_this->p_libvlc, NULL, NULL);
    vlc_mutex_destroy(&sys->msg_lock);
    vlc_mutex_destroy(&sys->pl_lock);
    for(unsigned i = 0; i < sizeof sys->msgs / sizeof *sys->msgs; i++) {
        if (sys->msgs[i].item)
            msg_Free(sys->msgs[i].item);
        free(sys->msgs[i].msg);
    }
    free(sys);
}
