}
static void HandleCommonKey(intf_thread_t *intf, int key)
{
    intf_sys_t *sys = intf->p_sys;
    playlist_t *p_playlist = pl_Get(intf);
    switch(key)
    {
    case 0x1b:  /* ESC */
        if (getch() != ERR)
            return;
    case 'q':
    case 'Q':
    case KEY_EXIT:
        libvlc_Quit(intf->p_libvlc);
        sys->exit = true;           // terminate the main loop
        return;
    case 'h':
    case 'H': BoxSwitch(sys, BOX_HELP);       return;
    case 'i': BoxSwitch(sys, BOX_INFO);       return;
    case 'M': BoxSwitch(sys, BOX_META);       return;
    case 'L': BoxSwitch(sys, BOX_LOG);        return;
    case 'P': BoxSwitch(sys, BOX_PLAYLIST);   return;
    case 'B': BoxSwitch(sys, BOX_BROWSE);     return;
    case 'x': BoxSwitch(sys, BOX_OBJECTS);    return;
    case 'S': BoxSwitch(sys, BOX_STATS);      return;
    case '/': /* Search */
        sys->plidx_follow = false;
        BoxSwitch(sys, BOX_SEARCH);
        return;
    case 'A': /* Open */
        sys->open_chain[0] = '\0';
        BoxSwitch(sys, BOX_OPEN);
        return;
    /* Navigation */
    case KEY_RIGHT: ChangePosition(intf, +0.01); return;
    case KEY_LEFT:  ChangePosition(intf, -0.01); return;
    /* Common control */
    case 'f':
        if (sys->p_input) {
            vout_thread_t *p_vout = input_GetVout(sys->p_input);
            if (p_vout) {
                bool fs = var_ToggleBool(p_playlist, "fullscreen");
                var_SetBool(p_vout, "fullscreen", fs);
                vlc_object_release(p_vout);
            }
        }
        return;
    case ' ': PlayPause(intf);            return;
    case 's': playlist_Stop(p_playlist);    return;
    case 'e': Eject(intf);                return;
    case '[': InputNavigate(sys->p_input, "prev-title");      return;
    case ']': InputNavigate(sys->p_input, "next-title");      return;
    case '<': InputNavigate(sys->p_input, "prev-chapter");    return;
    case '>': InputNavigate(sys->p_input, "next-chapter");    return;
    case 'p': playlist_Prev(p_playlist);            break;
    case 'n': playlist_Next(p_playlist);            break;
    case 'a': playlist_VolumeUp(p_playlist, 1, NULL);   break;
    case 'z': playlist_VolumeDown(p_playlist, 1, NULL); break;
    case 'm': playlist_MuteToggle(p_playlist); break;
    case 'c': CycleESTrack(sys, "audio-es"); break;
    case 'v': CycleESTrack(sys, "spu-es");   break;
    case 'b': CycleESTrack(sys, "video-es"); break;
    case 0x0c:  /* ^l */
    case KEY_CLEAR:
        break;
    default:
        return;
    }
    clear();
    return;
}
