static void sanitize_channel_layouts(void *log, AVFilterChannelLayouts *l)
{
    if (!l)
        return;
    if (l->nb_channel_layouts) {
        if (l->all_layouts || l->all_counts)
            av_log(log, AV_LOG_WARNING, "All layouts set on non-empty list\n");
        l->all_layouts = l->all_counts = 0;
    } else {
        if (l->all_counts && !l->all_layouts)
            av_log(log, AV_LOG_WARNING, "All counts without all layouts\n");
        l->all_layouts = 1;
    }
}
