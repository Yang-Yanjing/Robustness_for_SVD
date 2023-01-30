}
static vlc_v4l2_ctrl_t *ControlAddBitMask (vlc_object_t *obj, int fd,
                                           const struct v4l2_queryctrl *query)
{
    msg_Dbg (obj, " bit mask %s (%08"PRIX32")", query->name, query->id);
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
    vlc_value_t val;
    struct v4l2_control ctrl = { .id = query->id };
    if (v4l2_ioctl (fd, VIDIOC_G_CTRL, &ctrl) >= 0)
    {
        msg_Dbg (obj, "  current: 0x%08"PRIX32", default: 0x%08"PRIX32,
                 ctrl.value, query->default_value);
        val.i_int = ctrl.value;
        var_Change (obj, c->name, VLC_VAR_SETVALUE, &val, NULL);
    }
    val.i_int = 0;
    var_Change (obj, c->name, VLC_VAR_SETMIN, &val, NULL);
    val.i_int = (uint32_t)query->maximum;
    var_Change (obj, c->name, VLC_VAR_SETMAX, &val, NULL);
    val.i_int = query->default_value;
    var_Change (obj, c->name, VLC_VAR_SETDEFAULT, &val, NULL);
    return c;
}
