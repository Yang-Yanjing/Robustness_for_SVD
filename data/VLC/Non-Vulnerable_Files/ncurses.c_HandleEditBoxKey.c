}
static void HandleEditBoxKey(intf_thread_t *intf, int key, int box)
{
    intf_sys_t *sys = intf->p_sys;
    bool search = box == BOX_SEARCH;
    char *str = search ? sys->search_chain: sys->open_chain;
    size_t len = strlen(str);
    assert(box == BOX_SEARCH || box == BOX_OPEN);
    switch(key)
    {
    case 0x0c:  /* ^l */
    case KEY_CLEAR:     clear(); return;
    case KEY_ENTER:
    case '\r':
    case '\n':
        if (search)
            SearchPlaylist(sys);
        else
            OpenSelection(intf);
        sys->box_type = BOX_PLAYLIST;
        return;
    case 0x1b: /* ESC */
        /* Alt+key combinations return 2 keys in the terminal keyboard:
         * ESC, and the 2nd key.
         * If some other key is available immediately (where immediately
         * means after getch() 1 second delay), that means that the
         * ESC key was not pressed.
         *
         * man 3X curs_getch says:
         *
         * Use of the escape key by a programmer for a single
         * character function is discouraged, as it will cause a delay
         * of up to one second while the keypad code looks for a
         * following function-key sequence.
         *
         */
        if (getch() == ERR)
            sys->box_type = BOX_PLAYLIST;
        return;
    case KEY_BACKSPACE:
    case 0x7f:
        RemoveLastUTF8Entity(str, len);
        break;
    default:
        if (len + 1 < (search ? sizeof sys->search_chain
                              : sizeof sys->open_chain)) {
            str[len + 0] = key;
            str[len + 1] = '\0';
        }
    }
    if (search)
        SearchPlaylist(sys);
}
