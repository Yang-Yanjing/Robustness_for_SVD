}
gboolean update_progressbar(char *handle) {
    float pos = libvlc_vlm_get_media_instance_position(vlcinst, handle, 0);
    char *pos_string;
    if(pos < 1.0 && 0.0 <= pos) {
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar), pos);
        pos_string = get_pos_string(pos);
        if(pos_string != NULL) {
            gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressbar), pos_string);
            free(pos_string);
        }
        return TRUE;
    }
    if(stopped = 1) {
        free(handle);
        return FALSE;
    }
}
