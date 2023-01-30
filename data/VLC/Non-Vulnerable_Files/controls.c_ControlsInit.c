 */
vlc_v4l2_ctrl_t *ControlsInit (vlc_object_t *obj, int fd)
{
    /* A list of controls that can be modified at run-time is stored in the
     * "controls" variable. The V4L2 controls dialog can be built from this. */
    var_Create (obj, "controls", VLC_VAR_INTEGER | VLC_VAR_HASCHOICE);
    static const ctrl_type_cb handlers[] =
    {
        [V4L2_CTRL_TYPE_INTEGER] = ControlAddInteger,
        [V4L2_CTRL_TYPE_BOOLEAN] = ControlAddBoolean,
        [V4L2_CTRL_TYPE_MENU] = ControlAddMenu,
        [V4L2_CTRL_TYPE_BUTTON] = ControlAddButton,
        [V4L2_CTRL_TYPE_INTEGER64] = ControlAddInteger64,
        [V4L2_CTRL_TYPE_CTRL_CLASS] = ControlAddClass,
        [V4L2_CTRL_TYPE_STRING] = ControlAddString,
        [V4L2_CTRL_TYPE_BITMASK] = ControlAddBitMask,
        [V4L2_CTRL_TYPE_INTEGER_MENU] = ControlAddIntMenu,
    };
    vlc_v4l2_ctrl_t *list = NULL;
    struct v4l2_queryctrl query;
    query.id = V4L2_CTRL_FLAG_NEXT_CTRL;
    while (v4l2_ioctl (fd, VIDIOC_QUERYCTRL, &query) >= 0)
    {
        ctrl_type_cb handler = NULL;
        if (query.type < (sizeof (handlers) / sizeof (handlers[0])))
            handler = handlers[query.type];
        if (handler == NULL)
            handler = ControlAddUnknown;
        vlc_v4l2_ctrl_t *c = handler (obj, fd, &query);
        if (c != NULL)
        {
            vlc_value_t val, text;
            var_AddCallback (obj, c->name, ControlSetCallback, c);
            text.psz_string = (char *)query.name;
            var_Change (obj, c->name, VLC_VAR_SETTEXT, &text, NULL);
            val.i_int = query.id;
            text.psz_string = (char *)c->name;
            var_Change (obj, "controls", VLC_VAR_ADDCHOICE, &val, &text);
            c->next = list;
            list = c;
        }
        query.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
    }
    /* Set well-known controls from VLC configuration */
    for (vlc_v4l2_ctrl_t *ctrl = list; ctrl != NULL; ctrl = ctrl->next)
    {
        if (!CTRL_CID_KNOWN (ctrl->id))
            continue;
        char varname[sizeof (CFG_PREFIX) + sizeof (ctrl->name) - 1];
        sprintf (varname, CFG_PREFIX"%s", ctrl->name);
        int64_t val = var_InheritInteger (obj, varname);
        if (val == -1)
            continue; /* the VLC default value: "do not modify" */
        ControlSet (ctrl, val); /* NOTE: all known are integers or booleans */
    }
    /* Set any control from the VLC configuration control string */
    ControlsSetFromString (obj, list);
    /* Add a control to reset all controls to their default values */
    {
        vlc_value_t val, text;
        var_Create (obj, "reset", VLC_VAR_VOID | VLC_VAR_ISCOMMAND);
        val.psz_string = _("Reset defaults");
        var_Change (obj, "reset", VLC_VAR_SETTEXT, &val, NULL);
        val.i_int = -1;
        text.psz_string = (char *)"reset";
        var_Change (obj, "controls", VLC_VAR_ADDCHOICE, &val, &text);
        var_AddCallback (obj, "reset", ControlsResetCallback, list);
    }
    if (var_InheritBool (obj, CFG_PREFIX"controls-reset"))
        ControlsReset (obj, list);
    return list;
}
