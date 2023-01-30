}
gboolean on_error(void) {
    GtkWidget *dialog;
    stop();
    dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Error");gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return FALSE;
}
