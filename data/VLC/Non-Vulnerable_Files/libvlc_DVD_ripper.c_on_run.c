}
void on_run(GtkWidget *widget, gpointer data) {
    char *handle;
    const char *transcode;
    char *source = (char*) gtk_entry_get_text(GTK_ENTRY(source_entry));
    char *dest = (char*) gtk_entry_get_text(GTK_ENTRY(dest_entry));
    char *preset= (char*) gtk_combo_box_get_active_text(GTK_COMBO_BOX(format_chooser));
    int i;
    char file_begin[] = ":file{dst=";
    char file_end[] = "}";
    char *sout;
    if(preset == NULL) return;
    gtk_widget_set_sensitive(widget, FALSE);
    handle = malloc((strlen(source)+4+1) * sizeof(char));
    if(handle == NULL) return;
    strncpy(handle, source, strlen(source));
    for(i=0;i<=3;++i) {
        handle[strlen(source)+i] = (char) (((unsigned int) rand()) & 63) + '0';
    }
    handle[strlen(source)+4] = '\0';
    transcode = get_transcode_string(preset);
    free(preset);
    sout = malloc((strlen(transcode)+strlen(file_begin)+strlen(dest)+strlen(file_end)+1) * sizeof(char));
    if(sout == NULL) return;
    strncpy(sout, transcode, strlen(transcode)+1);
    strncat(sout, file_begin, strlen(file_begin));
    strncat(sout, dest, strlen(dest));
    strncat(sout, file_end, strlen(file_end));
    libvlc_vlm_add_broadcast(vlcinst, handle, source, sout, 0, NULL, 1, 0);
    free(sout);
    libvlc_vlm_play_media(vlcinst, handle);
    gtk_widget_show(spinner);
    gtk_spinner_start(GTK_SPINNER(spinner));
    stopped = 0;
    g_timeout_add(50, (GSourceFunc) update_progressbar, handle);
}
