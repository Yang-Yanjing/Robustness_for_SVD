}
void on_error_vlc(const libvlc_event_t *event, void *data) {
    g_idle_add((GSourceFunc) on_error, NULL);
}
