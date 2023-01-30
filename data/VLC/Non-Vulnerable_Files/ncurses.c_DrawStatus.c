}
static int DrawStatus(intf_thread_t *intf)
{
    intf_sys_t     *sys = intf->p_sys;
    input_thread_t *p_input = sys->p_input;
    playlist_t     *p_playlist = pl_Get(intf);
    char *name = _("VLC media player");
    const size_t name_len = strlen(name) + sizeof(PACKAGE_VERSION);
    int y = 0;
    const char *repeat, *loop, *random;
    /* Title */
    int padding = COLS - name_len; /* center title */
    if (padding < 0)
        padding = 0;
    attrset(A_REVERSE);
    if (sys->color) color_set(C_TITLE, NULL);
    DrawEmptyLine(y, 0, COLS);
    mvnprintw(y++, padding / 2, COLS, "%s %s", name, PACKAGE_VERSION);
    if (sys->color) color_set(C_STATUS, NULL);
    attroff(A_REVERSE);
    y++; /* leave a blank line */
    repeat = var_GetBool(p_playlist, "repeat") ? _("[Repeat] ") : "";
    random = var_GetBool(p_playlist, "random") ? _("[Random] ") : "";
    loop   = var_GetBool(p_playlist, "loop")   ? _("[Loop]")    : "";
    if (p_input && !p_input->b_dead) {
        vlc_value_t val;
        char *path, *uri;
        uri = input_item_GetURI(input_GetItem(p_input));
        path = make_path(uri);
        mvnprintw(y++, 0, COLS, _(" Source   : %s"), path?path:uri);
        free(uri);
        free(path);
        var_Get(p_input, "state", &val);
        switch(val.i_int)
        {
            static const char *input_state[] = {
                [PLAYING_S] = " State    : Playing %s%s%s",
                [OPENING_S] = " State    : Opening/Connecting %s%s%s",
                [PAUSE_S]   = " State    : Paused %s%s%s",
            };
            char buf1[MSTRTIME_MAX_SIZE];
            char buf2[MSTRTIME_MAX_SIZE];
            float volume;
        case INIT_S:
        case END_S:
            y += 2;
            break;
        case PLAYING_S:
        case OPENING_S:
        case PAUSE_S:
            mvnprintw(y++, 0, COLS, _(input_state[val.i_int]),
                        repeat, random, loop);
        default:
            var_Get(p_input, "time", &val);
            secstotimestr(buf1, val.i_time / CLOCK_FREQ);
            var_Get(p_input, "length", &val);
            secstotimestr(buf2, val.i_time / CLOCK_FREQ);
            mvnprintw(y++, 0, COLS, _(" Position : %s/%s"), buf1, buf2);
            volume = playlist_VolumeGet(p_playlist);
            int mute = playlist_MuteGet(p_playlist);
            mvnprintw(y++, 0, COLS,
                      mute ? _(" Volume   : Mute") :
                      volume >= 0.f ? _(" Volume   : %3ld%%") : _(" Volume   : ----"),
                      lroundf(volume * 100.f));
            if (!var_Get(p_input, "title", &val)) {
                int i_title_count = var_CountChoices(p_input, "title");
                if (i_title_count > 0)
                    mvnprintw(y++, 0, COLS, _(" Title    : %"PRId64"/%d"),
                               val.i_int, i_title_count);
            }
            if (!var_Get(p_input, "chapter", &val)) {
                int i_chapter_count = var_CountChoices(p_input, "chapter");
                if (i_chapter_count > 0) mvnprintw(y++, 0, COLS, _(" Chapter  : %"PRId64"/%d"),
                               val.i_int, i_chapter_count);
            }
        }
    } else {
        mvnprintw(y++, 0, COLS, _(" Source: <no current item> "));
        mvnprintw(y++, 0, COLS, " %s%s%s", repeat, random, loop);
        mvnprintw(y++, 0, COLS, _(" [ h for help ]"));
        DrawEmptyLine(y++, 0, COLS);
    }
    if (sys->color) color_set(C_DEFAULT, NULL);
    DrawBox(y++, 1, sys->color, ""); /* position slider */
    DrawEmptyLine(y, 1, COLS-2);
    if (p_input)
        DrawLine(y, 1, (int)((COLS-2) * var_GetFloat(p_input, "position")));
    y += 2; /* skip slider and box */
    return y;
}
