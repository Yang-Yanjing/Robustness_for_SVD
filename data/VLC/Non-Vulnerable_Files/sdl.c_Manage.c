 */
static void Manage(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    SDL_Event event;
    /* */
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            vout_display_SendEventClose(vd);
            break;
        case SDL_KEYDOWN: {
            /* convert the key if possible */
            int key = ConvertKey(event.key.keysym.sym);
            if (!key) {
                /* Find the right caracter */
                if ((event.key.keysym.unicode & 0xff80) == 0) {
                    key = event.key.keysym.unicode & 0x7f;
                    /* FIXME: find a better solution than this
                              hack to find the right caracter */
                    if (key >= 1 && key <= 26)
                        key += 96;
                    else if (key >= 65 && key <= 90)
                        key += 32;
                }
            }
            if (!key)
                break;
            if (event.key.keysym.mod & KMOD_SHIFT)
                key |= KEY_MODIFIER_SHIFT;
            if (event.key.keysym.mod & KMOD_CTRL)
                key |= KEY_MODIFIER_CTRL;
            if (event.key.keysym.mod & KMOD_ALT)
                key |= KEY_MODIFIER_ALT;
            vout_display_SendEventKey(vd, key);
            break;
        }
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP: {
            static const struct { int sdl; int vlc; } buttons[] = {
                { SDL_BUTTON_LEFT,      MOUSE_BUTTON_LEFT },
                { SDL_BUTTON_MIDDLE,    MOUSE_BUTTON_CENTER },
                { SDL_BUTTON_RIGHT,     MOUSE_BUTTON_RIGHT },
                { SDL_BUTTON_WHEELUP,   MOUSE_BUTTON_WHEEL_UP },
                { SDL_BUTTON_WHEELDOWN, MOUSE_BUTTON_WHEEL_DOWN },
                { -1, -1 },
            };
            SDL_ShowCursor(1);
            for (int i = 0; buttons[i].sdl != -1; i++) {
                if (buttons[i].sdl == event.button.button) {
                    if (event.type == SDL_MOUSEBUTTONDOWN)
                        vout_display_SendEventMousePressed(vd, buttons[i].vlc);
                    else
                        vout_display_SendEventMouseReleased(vd, buttons[i].vlc);
                }
            }
            break;
        }
        case SDL_MOUSEMOTION: {
            if (sys->place.width <= 0 || sys->place.height <= 0)
                break;
            SDL_ShowCursor(1);
            vout_display_SendMouseMovedDisplayCoordinates(vd, ORIENT_NORMAL,
                                                          event.motion.x, event.motion.y,
                                                          &sys->place);
            break;
        }
        case SDL_VIDEORESIZE:
            vout_display_SendEventDisplaySize(vd, event.resize.w, event.resize.h, vd->cfg->is_fullscreen);
            break;
        default:
            break;
        }
    }
