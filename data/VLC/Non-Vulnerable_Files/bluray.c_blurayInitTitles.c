}
static void blurayInitTitles(demux_t *p_demux, int menu_titles)
{
    demux_sys_t *p_sys = p_demux->p_sys;
#if BLURAY_VERSION < BLURAY_VERSION_CODE(0,5,0)
    int64_t duration = 0;
#endif
    /* get and set the titles */
    unsigned i_title = menu_titles;
    if (!p_sys->b_menu) {
        i_title = bd_get_titles(p_sys->bluray, TITLES_RELEVANT, 60);
#if BLURAY_VERSION >= BLURAY_VERSION_CODE(0,5,0)
        p_sys->i_longest_title = bd_get_main_title(p_sys->bluray);
#endif
    }
    for (unsigned int i = 0; i < i_title; i++) {
        input_title_t *t = vlc_input_title_New();
        if (!t)
            break;
        if (!p_sys->b_menu) {
            BLURAY_TITLE_INFO *title_info = bd_get_title_info(p_sys->bluray, i, 0);
            blurayUpdateTitleInfo(t, title_info);
            bd_free_title_info(title_info);
#if BLURAY_VERSION < BLURAY_VERSION_CODE(0,5,0)
            if (t->i_length > duration) {
                duration = t->i_length;
                p_sys->i_longest_title = i;
            }
#endif
        } else if (i == 0) {
            t->psz_name = strdup(_("Top Menu"));
        } else if (i == i_title - 1) {
            t->psz_name = strdup(_("First Play"));
        }
        TAB_APPEND(p_sys->i_title, p_sys->pp_title, t);
    }
}
