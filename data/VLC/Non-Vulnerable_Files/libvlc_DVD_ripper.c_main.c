}
int main (int argc, char *argv[]) {
    GtkWidget *media_list, *scrolled;
    GtkWidget *choose_frame, *output_frame, *run_frame;
    GtkWidget *vbox, *choose_table, *output_table, *run_table;
    GtkWidget *source_label, *source_button, *source_folder_button;
    GtkWidget *dest_label, *dest_button;
    libvlc_event_manager_t *evtman;
    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(window), BORDER_WIDTH);
    gtk_window_set_title(GTK_WINDOW(window), "libVLC DVD ripper");
    //setup vbox
    vbox = gtk_vbox_new(FALSE, BORDER_WIDTH);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    // setup media list with scrolled window
    media_list = gtk_tree_view_new();
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(media_list), FALSE);
    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled), media_list);
    // setup "choose"-frame
    choose_frame = gtk_frame_new("Choose DVD");
    gtk_box_pack_start(GTK_BOX(vbox), choose_frame, TRUE, TRUE, 0);
    choose_table = gtk_table_new(1, 4, FALSE);
    gtk_table_set_row_spacings(GTK_TABLE(choose_table), BORDER_WIDTH/2);
    gtk_table_set_col_spacings(GTK_TABLE(choose_table), BORDER_WIDTH/2);
    gtk_container_set_border_width(GTK_CONTAINER(choose_table), BORDER_WIDTH);
    source_label = gtk_label_new("Input file or folder:");
    source_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(source_entry), "dvd://");
    source_button = gtk_button_new_with_label("Open file");
    source_folder_button = gtk_button_new_with_label("Open folder");
    g_signal_connect(G_OBJECT(source_button), "clicked", G_CALLBACK(on_select_source_path), NULL);
    g_signal_connect(G_OBJECT(source_folder_button), "clicked", G_CALLBACK(on_select_source_path), (gpointer) 1);
    gtk_table_attach(GTK_TABLE(choose_table), source_label, 0, 1, 0, 1, GTK_SHRINK, GTK_SHRINK, 0, 0);
    gtk_table_attach(GTK_TABLE(choose_table), source_entry, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
    gtk_table_attach(GTK_TABLE(choose_table), source_button, 2, 3, 0, 1, GTK_SHRINK, GTK_SHRINK, 0, 0);
    gtk_table_attach(GTK_TABLE(choose_table), source_folder_button, 3, 4, 0, 1, GTK_SHRINK, GTK_SHRINK, 0, 0);
    gtk_container_add(GTK_CONTAINER(choose_frame), choose_table);
    // setup "output"-frame
    output_frame = gtk_frame_new("Output settings");
    gtk_box_pack_start(GTK_BOX(vbox), output_frame, TRUE, TRUE, 0);
    output_table = gtk_table_new(2, 3, FALSE);
    gtk_table_set_row_spacings(GTK_TABLE(output_table), BORDER_WIDTH/2);
    gtk_table_set_col_spacings(GTK_TABLE(output_table), BORDER_WIDTH/2);
    gtk_container_set_border_width(GTK_CONTAINER(output_table), BORDER_WIDTH);
    gtk_container_add(GTK_CONTAINER(output_frame), output_table);
    dest_label = gtk_label_new("Output file:");
    dest_entry = gtk_entry_new();
    format_chooser = gtk_combo_box_new_text();
    gtk_combo_box_append_text(GTK_COMBO_BOX(format_chooser), PRESET_H264_AAC_MP4_HIGH);
    gtk_combo_box_append_text(GTK_COMBO_BOX(format_chooser), PRESET_H264_AAC_MP4_LOW);
    gtk_combo_box_append_text(GTK_COMBO_BOX(format_chooser), PRESET_THEORA_VORBIS_OGG_HIGH);
    gtk_combo_box_append_text(GTK_COMBO_BOX(format_chooser), PRESET_THEORA_VORBIS_OGG_LOW);
    gtk_combo_box_append_text(GTK_COMBO_BOX(format_chooser), PRESET_VP8_VORBIS_WEBM_HIGH);
    gtk_combo_box_append_text(GTK_COMBO_BOX(format_chooser), PRESET_VP8_VORBIS_WEBM_LOW);
    gtk_combo_box_set_active(GTK_COMBO_BOX(format_chooser), 0);
    dest_button = gtk_button_new_from_stock(GTK_STOCK_OPEN);
    g_signal_connect(G_OBJECT(dest_button), "clicked", G_CALLBACK(on_select_dest_path), NULL);
    gtk_table_attach(GTK_TABLE(output_table), dest_label, 0, 1, 0, 1, GTK_SHRINK, GTK_SHRINK, 0, 0);
    gtk_table_attach(GTK_TABLE(output_table), dest_entry, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
    gtk_table_attach(GTK_TABLE(output_table), dest_button, 2, 3, 0, 1, GTK_SHRINK, GTK_SHRINK, 0, 0);
    gtk_table_attach(GTK_TABLE(output_table), format_chooser, 0, 4, 1, 2, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
    // setup "run"-frame
    run_frame = gtk_frame_new("Run");
    gtk_box_pack_start(GTK_BOX(vbox), run_frame, TRUE, TRUE, 0);
    run_table = gtk_table_new(3, 2, FALSE);
    gtk_table_set_row_spacings(GTK_TABLE(run_table), BORDER_WIDTH/2);
    gtk_table_set_col_spacings(GTK_TABLE(run_table), BORDER_WIDTH/2);
    gtk_container_set_border_width(GTK_CONTAINER(run_table), BORDER_WIDTH);
    gtk_container_add(GTK_CONTAINER(run_frame), run_table);
    progressbar = gtk_progress_bar_new();
    spinner = gtk_spinner_new();
    run_button = gtk_button_new_from_stock(GTK_STOCK_OK);
    g_signal_connect(G_OBJECT(run_button), "clicked", G_CALLBACK(on_run), NULL);
    gtk_table_attach(GTK_TABLE(run_table), progressbar, 0, 3, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
    gtk_table_attach(GTK_TABLE(run_table), gtk_label_new(""), 0, 1, 1, 2, GTK_EXPAND, GTK_SHRINK, 0, 0);
    gtk_table_attach(GTK_TABLE(run_table), spinner, 1, 2, 1, 2, GTK_SHRINK, GTK_SHRINK, 0, 0);
    gtk_table_attach(GTK_TABLE(run_table), run_button, 2, 3, 1, 2, GTK_SHRINK, GTK_SHRINK, 0, 0);
    // setup vlc
    vlcinst = libvlc_new(0, NULL);
    evtman = libvlc_vlm_get_event_manager(vlcinst);
	libvlc_event_attach(evtman, libvlc_VlmMediaInstanceStatusEnd, on_end_vlc, NULL);
	libvlc_event_attach(evtman, libvlc_VlmMediaInstanceStatusError, on_error_vlc, NULL);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(window);
    gtk_widget_hide(spinner);
    gtk_main ();
    libvlc_release(vlcinst);
    return 0;
}
