 */
static void Manage(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    for (;;) {
        const int event = aa_getevent(sys->aa_context, 0);
        if (!event)
            return;
        switch (event) {
        case AA_MOUSE: {
            int x, y;
            int button;
            int vlc;
            aa_getmouse(sys->aa_context, &x, &y, &button);
            vlc = 0;
            if (button & AA_BUTTON1)
                vlc |= 1 << MOUSE_BUTTON_LEFT;
            if (button & AA_BUTTON2)
                vlc |= 1 << MOUSE_BUTTON_CENTER;
            if (button & AA_BUTTON3)
                vlc |= 1 << MOUSE_BUTTON_RIGHT;
            vout_display_SendEventMouseState(vd, x, y, vlc);
            aa_showcursor(sys->aa_context); /* Not perfect, we show it on click too */
            break;
        }
        case AA_RESIZE:
            aa_resize(sys->aa_context);
            vout_display_SendEventDisplaySize(vd,
                                              aa_imgwidth(sys->aa_context),
                                              aa_imgheight(sys->aa_context), false);
            break;
        /* TODO keys support to complete */
        case AA_UP:
            vout_display_SendEventKey(vd, KEY_UP);
            break;
        case AA_DOWN:
            vout_display_SendEventKey(vd, KEY_DOWN);
            break;
        case AA_RIGHT:
            vout_display_SendEventKey(vd, KEY_RIGHT);
            break;
        case AA_LEFT:
            vout_display_SendEventKey(vd, KEY_LEFT);
            break;
        case AA_BACKSPACE:
            vout_display_SendEventKey(vd, KEY_BACKSPACE);
            break;
        case AA_ESC:
            vout_display_SendEventKey(vd, KEY_ESC);
            break;
        default:
            if (event >= 0x20 && event <= 0x7f)
                vout_display_SendEventKey(vd, event);
            break;
        }
    }
}
