}
void on_end_vlc(const libvlc_event_t *event, void *data) {
    g_idle_add((GSourceFunc) on_end, NULL);
}
