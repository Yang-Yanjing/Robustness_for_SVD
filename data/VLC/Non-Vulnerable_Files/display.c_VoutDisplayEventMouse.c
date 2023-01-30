}
static void VoutDisplayEventMouse(vout_display_t *vd, int event, va_list args)
{
    vout_display_owner_sys_t *osys = vd->owner.sys;
    vlc_mutex_lock(&osys->lock);
    /* */
    vlc_mouse_t m = osys->mouse.state;
    bool is_ignored = false;
    switch (event) {
    case VOUT_DISPLAY_EVENT_MOUSE_STATE: {
        const int x = (int)va_arg(args, int);
        const int y = (int)va_arg(args, int);
        const int button_mask = (int)va_arg(args, int);
        vlc_mouse_Init(&m);
        m.i_x = x;
        m.i_y = y;
        m.i_pressed = button_mask;
        break;
    }
    case VOUT_DISPLAY_EVENT_MOUSE_MOVED: {
        const int x = (int)va_arg(args, int);
        const int y = (int)va_arg(args, int);
        //msg_Dbg(vd, "VoutDisplayEvent 'mouse' @%d,%d", x, y);
        m.i_x = x;
        m.i_y = y;
        m.b_double_click = false;
        break;
    }
    case VOUT_DISPLAY_EVENT_MOUSE_PRESSED:
    case VOUT_DISPLAY_EVENT_MOUSE_RELEASED: {
        const int button = (int)va_arg(args, int);
        const int button_mask = 1 << button;
        /* Ignore inconsistent event */
        if ((event == VOUT_DISPLAY_EVENT_MOUSE_PRESSED  &&  (osys->mouse.state.i_pressed & button_mask)) ||
            (event == VOUT_DISPLAY_EVENT_MOUSE_RELEASED && !(osys->mouse.state.i_pressed & button_mask))) {
            is_ignored = true;
            break;
        }
        /* */
        msg_Dbg(vd, "VoutDisplayEvent 'mouse button' %d t=%d", button, event);
        m.b_double_click = false;
        if (event == VOUT_DISPLAY_EVENT_MOUSE_PRESSED)
            m.i_pressed |= button_mask;
        else
            m.i_pressed &= ~button_mask;
        break;
    }
    case VOUT_DISPLAY_EVENT_MOUSE_DOUBLE_CLICK:
        msg_Dbg(vd, "VoutDisplayEvent 'double click'");
        m.b_double_click = true;
        break;
    default:
        assert(0);
    }
    if (is_ignored) {
        vlc_mutex_unlock(&osys->lock);
        return;
    }
    /* Emulate double-click if needed */
    if (!vd->info.has_double_click &&
        vlc_mouse_HasPressed(&osys->mouse.state, &m, MOUSE_BUTTON_LEFT)) {
        const mtime_t i_date = mdate();
        if (i_date - osys->mouse.last_pressed < osys->mouse.double_click_timeout ) {
            m.b_double_click = true;
            osys->mouse.last_pressed = 0;
        } else {
            osys->mouse.last_pressed = mdate();
        }
    }
    /* */
    osys->mouse.state = m;
    /* */
    osys->mouse.ch_activity = true;
    if (!vd->info.has_hide_mouse)
        osys->mouse.last_moved = mdate();
    /* */
    vout_SendEventMouseVisible(osys->vout);
#ifdef ALLOW_DUMMY_VOUT
    DummyVoutSendDisplayEventMouse(osys->vout, &osys->vout_mouse, &m);
#else
    vout_SendDisplayEventMouse(osys->vout, &m);
#endif
    vlc_mutex_unlock(&osys->lock);
}
