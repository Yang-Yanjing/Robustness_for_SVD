}
static int Control (vout_display_t *vd, int query, va_list ap)
{
    vout_display_sys_t *sys = vd->sys;
    switch (query)
    {
      case VOUT_DISPLAY_HIDE_MOUSE: /* FIXME TODO */
        break;
#ifndef NDEBUG
      case VOUT_DISPLAY_RESET_PICTURES: // not needed
        assert(0);
#endif
      case VOUT_DISPLAY_CHANGE_FULLSCREEN:
      {
        const vout_display_cfg_t *cfg =
            va_arg (ap, const vout_display_cfg_t *);
        return vout_window_SetFullScreen (sys->window, cfg->is_fullscreen);
      }
      case VOUT_DISPLAY_CHANGE_WINDOW_STATE:
      {
        unsigned state = va_arg (ap, unsigned);
        return vout_window_SetState (sys->window, state);
      }
      case VOUT_DISPLAY_CHANGE_DISPLAY_SIZE:
      case VOUT_DISPLAY_CHANGE_DISPLAY_FILLED:
      case VOUT_DISPLAY_CHANGE_ZOOM:
      {
        const vout_display_cfg_t *cfg = va_arg (ap, const vout_display_cfg_t *);
        const video_format_t *src = &vd->source;
        if (query == VOUT_DISPLAY_CHANGE_DISPLAY_SIZE)
        {
            bool force = false;
            force = va_arg (ap, int);
            if (force
             && (cfg->display.width  != vd->cfg->display.width
              || cfg->display.height != vd->cfg->display.height)
             && vout_window_SetSize (sys->window,
                                     cfg->display.width, cfg->display.height))
                return VLC_EGENERIC;
        }
        vout_display_place_t place;
        vout_display_PlacePicture (&place, src, cfg, false);
        vlc_gl_MakeCurrent (sys->gl);
        glViewport (place.x, place.y, place.width, place.height);
        vlc_gl_ReleaseCurrent (sys->gl);
        return VLC_SUCCESS;
      }
      case VOUT_DISPLAY_CHANGE_SOURCE_ASPECT:
      case VOUT_DISPLAY_CHANGE_SOURCE_CROP:
      {
        const vout_display_cfg_t *cfg = vd->cfg;
        const video_format_t *src = va_arg (ap, const video_format_t *);
        vout_display_place_t place;
        vout_display_PlacePicture (&place, src, cfg, false);
        vlc_gl_MakeCurrent (sys->gl);
        glViewport (place.x, place.y, place.width, place.height);
        vlc_gl_ReleaseCurrent (sys->gl);
        return VLC_SUCCESS;
      }
      case VOUT_DISPLAY_GET_OPENGL:
      {
        vlc_gl_t **pgl = va_arg (ap, vlc_gl_t **);
        *pgl = sys->gl;
        return VLC_SUCCESS;
      }
      default:
        msg_Err (vd, "Unknown request %d", query);
    }
    return VLC_EGENERIC;
}
