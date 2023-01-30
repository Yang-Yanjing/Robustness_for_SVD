}
static void VoutDisplayEvent(vout_display_t *vd, int event, va_list args)
{
    vout_display_owner_sys_t *osys = vd->owner.sys;
    switch (event) {
    case VOUT_DISPLAY_EVENT_CLOSE: {
        msg_Dbg(vd, "VoutDisplayEvent 'close'");
        vout_SendEventClose(osys->vout);
        break;
    }
    case VOUT_DISPLAY_EVENT_KEY: {
        const int key = (int)va_arg(args, int);
        msg_Dbg(vd, "VoutDisplayEvent 'key' 0x%2.2x", key);
        if (vd->info.has_event_thread)
            vout_SendEventKey(osys->vout, key);
        else
            VoutDisplayEventKey(vd, key);
        break;
    }
    case VOUT_DISPLAY_EVENT_MOUSE_STATE:
    case VOUT_DISPLAY_EVENT_MOUSE_MOVED:
    case VOUT_DISPLAY_EVENT_MOUSE_PRESSED:
    case VOUT_DISPLAY_EVENT_MOUSE_RELEASED:
    case VOUT_DISPLAY_EVENT_MOUSE_DOUBLE_CLICK:
        VoutDisplayEventMouse(vd, event, args);
        break;
    case VOUT_DISPLAY_EVENT_FULLSCREEN: {
        const int is_fullscreen = (int)va_arg(args, int);
        msg_Dbg(vd, "VoutDisplayEvent 'fullscreen' %d", is_fullscreen);
        vlc_mutex_lock(&osys->lock);
        if (!is_fullscreen != !osys->is_fullscreen) {
            osys->ch_fullscreen = true;
            osys->is_fullscreen = is_fullscreen;
        }
        vlc_mutex_unlock(&osys->lock);
        break;
    }
    case VOUT_DISPLAY_EVENT_WINDOW_STATE: {
        const unsigned state = va_arg(args, unsigned);
        msg_Dbg(vd, "VoutDisplayEvent 'window state' %u", state);
        vlc_mutex_lock(&osys->lock);
        if (state != osys->wm_state) {
            osys->ch_wm_state = true;
            osys->wm_state = state;
        }
        vlc_mutex_unlock(&osys->lock);
        break;
    }
    case VOUT_DISPLAY_EVENT_DISPLAY_SIZE: {
        const int width  = (int)va_arg(args, int);
        const int height = (int)va_arg(args, int);
        const bool is_fullscreen = (bool)va_arg(args, int);
        msg_Dbg(vd, "VoutDisplayEvent 'resize' %dx%d %s",
                width, height, is_fullscreen ? "fullscreen" : "window");
        /* */
        vlc_mutex_lock(&osys->lock);
        osys->ch_display_size       = true;
        osys->display_width         = width;
        osys->display_height        = height;
        osys->display_is_fullscreen = is_fullscreen;
        osys->display_is_forced     = false;
        vlc_mutex_unlock(&osys->lock);
        break;
    }
    case VOUT_DISPLAY_EVENT_PICTURES_INVALID: {
        msg_Warn(vd, "VoutDisplayEvent 'pictures invalid'");
        /* */
        assert(vd->info.has_pictures_invalid);
        vlc_mutex_lock(&osys->lock);
        osys->reset_pictures = true;
        vlc_mutex_unlock(&osys->lock);
        break;
    }
    default:
        msg_Err(vd, "VoutDisplayEvent received event %d", event);
        /* TODO add an assert when all event are handled */
        break;
    }
}
