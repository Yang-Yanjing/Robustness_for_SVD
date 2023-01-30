}
static vlc_v4l2_ctrl_t *ControlAddBoolean (vlc_object_t *obj, int fd,
                                           const struct v4l2_queryctrl *query)
{
    msg_Dbg (obj, " boolean  %s (%08"PRIX32")", query->name, query->id);
    if (query->flags & CTRL_FLAGS_IGNORE)
        return NULL;
    vlc_v4l2_ctrl_t *c = ControlCreate (fd, query);
    if (unlikely(c == NULL))
        return NULL;
    if (var_Create (obj, c->name, VLC_VAR_BOOL | VLC_VAR_ISCOMMAND))
    {
        free (c);
        return NULL;
    }
    vlc_value_t val;
    struct v4l2_control ctrl = { .id = query->id };
    if (v4l2_ioctl (fd, VIDIOC_G_CTRL, &ctrl) >= 0)
    {
        msg_Dbg (obj, "  current: %s, default: %s",
                 ctrl.value ? " true" : "false",
                 query->default_value ? " true" : "false");
        val.b_bool = ctrl.value;
        var_Change (obj, c->name, VLC_VAR_SETVALUE, &val, NULL);
    }
    val.b_bool = query->default_value;
    var_Change (obj, c->name, VLC_VAR_SETDEFAULT, &val, NULL);
    return c;
}
