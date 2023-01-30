}
static vlc_v4l2_ctrl_t *ControlAddUnknown (vlc_object_t *obj, int fd,
                                           const struct v4l2_queryctrl *query)
{
    msg_Dbg (obj, " unknown %s (%08"PRIX32")", query->name, query->id);
    msg_Warn (obj, "  unknown control type %u", (unsigned)query->type);
    (void) fd;
    return NULL;
}
