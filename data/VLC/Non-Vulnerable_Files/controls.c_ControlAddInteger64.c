}
static vlc_v4l2_ctrl_t *ControlAddInteger64 (vlc_object_t *obj, int fd,
                                            const struct v4l2_queryctrl *query)
{
    msg_Dbg (obj, " 64-bits  %s (%08"PRIX32")", query->name, query->id);
    if (query->flags & CTRL_FLAGS_IGNORE)
        return NULL;
    vlc_v4l2_ctrl_t *c = ControlCreate (fd, query);
    if (unlikely(c == NULL))
        return NULL;
    if (var_Create (obj, c->name, VLC_VAR_INTEGER | VLC_VAR_ISCOMMAND))
    {
        free (c);
        return NULL;
    }
    struct v4l2_ext_control ext_ctrl = { .id = c->id, .size = 0, };
    struct v4l2_ext_controls ext_ctrls = {
        .ctrl_class = V4L2_CTRL_ID2CLASS(c->id),
        .count = 1,
        .error_idx = 0,
        .controls = &ext_ctrl,
    };
    if (v4l2_ioctl (c->fd, VIDIOC_G_EXT_CTRLS, &ext_ctrls) >= 0)
    {
        vlc_value_t val = { .i_int = ext_ctrl.value64 };
        msg_Dbg (obj, "  current: %"PRId64, val.i_int);
        var_Change (obj, c->name, VLC_VAR_SETVALUE, &val, NULL);
    }
    return c;
}
