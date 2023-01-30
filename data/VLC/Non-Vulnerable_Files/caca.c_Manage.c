 */
static void Manage(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    struct caca_event ev;
    while (caca_get_event(sys->dp, CACA_EVENT_ANY, &ev, 0) > 0) {
        switch (caca_get_event_type(&ev)) {
        case CACA_EVENT_KEY_PRESS: {
            const int caca = caca_get_event_key_ch(&ev);
            for (int i = 0; keys[i].caca != -1; i++) {
                if (keys[i].caca == caca) {
                    const int vlc = keys[i].vlc;
                    if (vlc >= 0)
                        vout_display_SendEventKey(vd, vlc);
                    return;
                }
            }
            if (caca >= 0x20 && caca <= 0x7f) {
                vout_display_SendEventKey(vd, caca);
                return;
            }
            break;
        }
        case CACA_EVENT_RESIZE:
            vout_display_SendEventDisplaySize(vd, caca_get_event_resize_width(&ev),
                                                  caca_get_event_resize_height(&ev), false);
            break;
        case CACA_EVENT_MOUSE_MOTION: {
            vout_display_place_t place;
            Place(vd, &place);
            const unsigned x = vd->source.i_x_offset +
                               (int64_t)(caca_get_event_mouse_x(&ev) - place.x) *
                                    vd->source.i_visible_width / place.width;
            const unsigned y = vd->source.i_y_offset +
                               (int64_t)(caca_get_event_mouse_y(&ev) - place.y) *
                                    vd->source.i_visible_height / place.height;
            caca_set_mouse(sys->dp, 1);
            vout_display_SendEventMouseMoved(vd, x, y);
            break;
        }
        case CACA_EVENT_MOUSE_PRESS:
        case CACA_EVENT_MOUSE_RELEASE: {
            caca_set_mouse(sys->dp, 1);
            const int caca = caca_get_event_mouse_button(&ev);
            for (int i = 0; mouses[i].caca != -1; i++) {
                if (mouses[i].caca == caca) {
                    if (caca_get_event_type(&ev) == CACA_EVENT_MOUSE_PRESS)
                        vout_display_SendEventMousePressed(vd, mouses[i].vlc);
                    else
                        vout_display_SendEventMouseReleased(vd, mouses[i].vlc);
                    return;
                }
            }
            break;
        }
        case CACA_EVENT_QUIT:
            vout_display_SendEventClose(vd);
            break;
        default:
            break;
        }
    }
}
