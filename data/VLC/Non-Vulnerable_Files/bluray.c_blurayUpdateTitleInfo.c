}
static void blurayUpdateTitleInfo(input_title_t *t, BLURAY_TITLE_INFO *title_info)
{
    t->i_length = FROM_TICKS(title_info->duration);
    if (!t->i_seekpoint) {
        for (unsigned int j = 0; j < title_info->chapter_count; j++) {
            seekpoint_t *s = vlc_seekpoint_New();
            if (!s) {
                break;
            }
            s->i_time_offset = title_info->chapters[j].offset;
            TAB_APPEND(t->i_seekpoint, t->seekpoint, s);
        }
    }
}
