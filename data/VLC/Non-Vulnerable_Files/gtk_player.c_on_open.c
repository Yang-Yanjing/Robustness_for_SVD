}
void on_open(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Choose Media", data, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *uri;
        uri = gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(dialog));
        open_media(uri);
        g_free(uri);
    }
    gtk_widget_destroy(dialog);
}
