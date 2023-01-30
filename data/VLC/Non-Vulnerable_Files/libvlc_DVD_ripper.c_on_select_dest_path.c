}
void on_select_dest_path(GtkWidget *widget, gpointer data) {
    gchar *path;
    path = get_filepath(widget, GTK_FILE_CHOOSER_ACTION_SAVE);
    if(path != NULL) {
        gtk_entry_set_text(GTK_ENTRY(dest_entry), path);
        g_free(path);
    }
}
