char stopped;
gchar* get_filepath(GtkWidget* widget, GtkFileChooserAction action) {
    GtkWidget *dialog;
    gchar *path;
    dialog = gtk_file_chooser_dialog_new("Choose location", GTK_WINDOW(gtk_widget_get_toplevel(widget)), action, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    }
    else {
        path = NULL;
    }
    gtk_widget_destroy(dialog);
    return path;
}
