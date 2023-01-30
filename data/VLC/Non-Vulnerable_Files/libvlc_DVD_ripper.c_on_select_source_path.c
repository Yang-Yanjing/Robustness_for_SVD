}
void on_select_source_path(GtkWidget *widget, gpointer data) {
    char *path;
    char scheme[] = "file://";
    char *uri;
    if(data==NULL) {
        path = (char*) get_filepath(widget, GTK_FILE_CHOOSER_ACTION_OPEN);
    }
    else {
        path = (char*) get_filepath(widget, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
    }
    if(path != NULL) {
        uri = malloc((strlen(scheme)+strlen(path)+1) * sizeof(char));
        if(uri == NULL) return;
        uri[0] = '\0';
        strncat(uri, scheme, strlen(scheme));
        strncat(uri, path, strlen(path));
        g_free(path);
        gtk_entry_set_text(GTK_ENTRY(source_entry), uri);
        free(uri);
    }
}
