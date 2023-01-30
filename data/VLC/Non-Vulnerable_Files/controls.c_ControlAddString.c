}
static vlc_v4l2_ctrl_t *ControlAddString (vlc_object_t *obj, int fd,
                                          const struct v4l2_queryctrl *query)
{
    msg_Dbg (obj, " string   %s (%08"PRIX32")", query->name, query->id);
    if ((query->flags & CTRL_FLAGS_IGNORE) || query->maximum > 65535)
        return NULL;
    vlc_v4l2_ctrl_t *c = ControlCreate (fd, query);
    if (unlikely(c == NULL))
        return NULL;
    if (var_Create (obj, c->name, VLC_VAR_STRING | VLC_VAR_ISCOMMAND))
    {
        free (c);
        return NULL;
    }
    /* Get current value */
    char *buf = malloc (query->maximum + 1);
    if (likely(buf != NULL))
    {
        struct v4l2_ext_control ext_ctrl = {
            .id = c->id,
            .size = query->maximum + 1,
        };
        ext_ctrl.string = buf;
        struct v4l2_ext_controls ext_ctrls = {
            .ctrl_class = V4L2_CTRL_ID2CLASS(c->id),
            .count = 1,
            .error_idx = 0,
            .controls = &ext_ctrl,
        };
        if (v4l2_ioctl (c->fd, VIDIOC_G_EXT_CTRLS, &ext_ctrls) >= 0)
        {
            vlc_value_t val = { .psz_string = buf };
            msg_Dbg (obj, "  current: \"%s\"", buf);
            var_Change (obj, c->name, VLC_VAR_SETVALUE, &val, NULL);
        }
        free (buf);
    }
    return c;
}
