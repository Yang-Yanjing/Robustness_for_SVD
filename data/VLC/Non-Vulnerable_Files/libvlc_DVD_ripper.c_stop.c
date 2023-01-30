}
void stop(void) {
    stopped = 1;
    gtk_spinner_stop(GTK_SPINNER(spinner));
    gtk_widget_hide(spinner);
    gtk_widget_set_sensitive(run_button, TRUE);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar), 0.0);
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressbar), "");
}
