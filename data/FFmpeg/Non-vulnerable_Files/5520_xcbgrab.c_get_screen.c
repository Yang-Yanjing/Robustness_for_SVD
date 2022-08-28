static xcb_screen_t *get_screen(const xcb_setup_t *setup, int screen_num)
{
    xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
    xcb_screen_t *screen     = NULL;
    for (; it.rem > 0; xcb_screen_next (&it)) {
        if (!screen_num) {
            screen = it.data;
            break;
        }
        screen_num--;
    }
    return screen;
}
