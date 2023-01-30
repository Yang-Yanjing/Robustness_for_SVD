}
static vlc_v4l2_ctrl_t *ControlAddButton (vlc_object_t *obj, int fd,
                                          const struct v4l2_queryctrl *query)
{
    msg_Dbg (obj, " button   %s (%08"PRIX32")", query->name, query->id);
    if (query->flags & CTRL_FLAGS_IGNORE)
        return NULL;
    vlc_v4l2_ctrl_t *c = ControlCreate (fd, query);
    if (unlikely(c == NULL))
        return NULL;
    if (var_Create (obj, c->name, VLC_VAR_VOID | VLC_VAR_ISCOMMAND))
    {
        free (c);
        return NULL;
    }
    return c;
}
