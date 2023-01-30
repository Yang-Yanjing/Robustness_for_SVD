}
int main( int argc, char *argv[] ) {
    GtkWidget *window,
              *vbox,
              *menubar,
              *filemenu,
              *fileitem,
              *filemenu_openitem,
              *player_widget,
              *hbuttonbox,
              *stop_button;
    gtk_init (&argc, &argv);
    // setup window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);
    gtk_container_set_border_width (GTK_CONTAINER (window), 0);
    gtk_window_set_title(GTK_WINDOW(window), "GTK+ libVLC Demo");
    //setup vbox
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    //setup menu
    menubar = gtk_menu_bar_new();
    filemenu = gtk_menu_new();
    fileitem = gtk_menu_item_new_with_label ("File");
    filemenu_openitem = gtk_menu_item_new_with_label("Open");
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), filemenu_openitem);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileitem), filemenu);
    gtk_menu_bar_append(GTK_MENU_BAR(menubar), fileitem);
    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
    g_signal_connect(filemenu_openitem, "activate", G_CALLBACK(on_open), window);
    //setup player widget
    player_widget = gtk_drawing_area_new();
    gtk_box_pack_start(GTK_BOX(vbox), player_widget, TRUE, TRUE, 0);
    //setup controls
    //playpause_button = gtk_button_new_from_stock(GTK_STOCK_MEDIA_PLAY);
    playpause_button = gtk_button_new_with_label("gtk-media-play");
    gtk_button_set_use_stock(GTK_BUTTON(playpause_button), TRUE);
    stop_button = gtk_button_new_from_stock(GTK_STOCK_MEDIA_STOP);
    g_signal_connect(playpause_button, "clicked", G_CALLBACK(on_playpause), NULL);
    g_signal_connect(stop_button, "clicked", G_CALLBACK(on_stop), NULL);
    hbuttonbox = gtk_hbutton_box_new();
    gtk_container_set_border_width(GTK_CONTAINER(hbuttonbox), BORDER_WIDTH);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(hbuttonbox), GTK_BUTTONBOX_START);
    gtk_box_pack_start(GTK_BOX(hbuttonbox), playpause_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbuttonbox), stop_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbuttonbox, FALSE, FALSE, 0);
    //setup vlc
    vlc_inst = libvlc_new(0, NULL);
    media_player = libvlc_media_player_new(vlc_inst);
    g_signal_connect(G_OBJECT(player_widget), "realize", G_CALLBACK(player_widget_on_realize), NULL);
    gtk_widget_show_all(window);
    gtk_main ();
    libvlc_media_player_release(media_player);
    libvlc_release(vlc_inst);
    return 0;
}
