}
static int ControlSetCallback (vlc_object_t *obj, const char *var,
                               vlc_value_t old, vlc_value_t cur, void *data)
{
    const vlc_v4l2_ctrl_t *ctrl = data;
    int ret;
    switch (ctrl->type)
    {
        case V4L2_CTRL_TYPE_INTEGER:
        case V4L2_CTRL_TYPE_MENU:
        case V4L2_CTRL_TYPE_BITMASK:
        case V4L2_CTRL_TYPE_INTEGER_MENU:
            ret = ControlSet (ctrl, cur.i_int);
            break;
        case V4L2_CTRL_TYPE_BOOLEAN:
            ret = ControlSet (ctrl, cur.b_bool);
            break;
        case V4L2_CTRL_TYPE_BUTTON:
            ret = ControlSet (ctrl, 0);
            break;
        case V4L2_CTRL_TYPE_INTEGER64:
            ret = ControlSet64 (ctrl, cur.i_int);
            break;
        case V4L2_CTRL_TYPE_STRING:
            ret = ControlSetStr (ctrl, cur.psz_string);
            break;
        default:
            assert (0);
    }
    if (ret)
    {
        msg_Err (obj, "cannot set control %s: %s", var, vlc_strerror_c(errno));
        return VLC_EGENERIC;
    }
    (void) old;
    return VLC_SUCCESS;
}
