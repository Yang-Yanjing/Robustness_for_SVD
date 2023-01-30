}
static vlc_v4l2_ctrl_t *ControlAddMenu (vlc_object_t *obj, int fd,
                                        const struct v4l2_queryctrl *query)
{
    msg_Dbg (obj, " menu     %s (%08"PRIX32")", query->name, query->id);
    if (query->flags & CTRL_FLAGS_IGNORE)
        return NULL;
    vlc_v4l2_ctrl_t *c = ControlCreate (fd, query);
    if (unlikely(c == NULL))
        return NULL;
    if (var_Create (obj, c->name, VLC_VAR_INTEGER | VLC_VAR_HASCHOICE
                                                  | VLC_VAR_ISCOMMAND))
    {
        free (c);
        return NULL;
    }
    vlc_value_t val;
    struct v4l2_control ctrl = { .id = query->id };
    if (v4l2_ioctl (fd, VIDIOC_G_CTRL, &ctrl) >= 0)
    {
        msg_Dbg (obj, "  current: %"PRId32", default: %"PRId32,
                 ctrl.value, query->default_value);
        val.i_int = ctrl.value;
        var_Change (obj, c->name, VLC_VAR_SETVALUE, &val, NULL);
    }
    val.i_int = query->minimum;
    var_Change (obj, c->name, VLC_VAR_SETMIN, &val, NULL);
    val.i_int = query->maximum;
    var_Change (obj, c->name, VLC_VAR_SETMAX, &val, NULL);
    val.i_int = query->default_value;
    var_Change (obj, c->name, VLC_VAR_SETDEFAULT, &val, NULL);
    /* Import menu choices */
    for (uint_fast32_t idx = query->minimum;
         idx <= (uint_fast32_t)query->maximum;
         idx++)
    {
        struct v4l2_querymenu menu = { .id = query->id, .index = idx };
        if (v4l2_ioctl (fd, VIDIOC_QUERYMENU, &menu) < 0)
            continue;
        msg_Dbg (obj, "  choice %"PRIu32") %s", menu.index, menu.name);
        vlc_value_t text;
        val.i_int = menu.index;
        text.psz_string = (char *)menu.name;
        var_Change (obj, c->name, VLC_VAR_ADDCHOICE, &val, &text);
    }
    return c;
}
