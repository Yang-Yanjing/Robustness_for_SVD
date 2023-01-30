}
static void ControlsSetFromString (vlc_object_t *obj,
                                   const vlc_v4l2_ctrl_t *list)
{
    char *buf = var_InheritString (obj, CFG_PREFIX"set-ctrls");
    if (buf == NULL)
        return;
    char *p = buf;
    if (*p == '{')
        p++;
    char *end = strchr (p, '}');
    if (end != NULL)
        *end = '\0';
next:
    while (p != NULL && *p)
    {
        const char *name, *value;
        p += strspn (p, ", ");
        name = p;
        end = strchr (p, ',');
        if (end != NULL)
            *(end++) = '\0';
        p = end; /* next name/value pair */
        end = strchr (name, '=');
        if (end == NULL)
        {
            /* TODO? support button controls that way? */
            msg_Err (obj, "syntax error in \"%s\": missing '='", name);
            continue;
        }
        *(end++) = '\0';
        value = end;
        for (const vlc_v4l2_ctrl_t *c = list; c != NULL; c = c->next)
            if (!strcasecmp (name, c->name))
                switch (c->type)
                {
                    case V4L2_CTRL_TYPE_INTEGER:
                    case V4L2_CTRL_TYPE_BOOLEAN:
                    case V4L2_CTRL_TYPE_MENU:
                    case V4L2_CTRL_TYPE_INTEGER_MENU:
                    {
                        long val = strtol (value, &end, 0);
                        if (*end)
                        {
                            msg_Err (obj, "syntax error in \"%s\": "
                                     " not an integer", value);
                            goto next;
                        }
                        ControlSet (c, val);
                        break;
                    }
                    case V4L2_CTRL_TYPE_INTEGER64:
                    {
                        long long val = strtoll (value, &end, 0);
                        if (*end)
                        {
                            msg_Err (obj, "syntax error in \"%s\": "
                                     " not an integer", value);
                            goto next;
                        }
                        ControlSet64 (c, val);
                        break;
                    }
                    case V4L2_CTRL_TYPE_STRING:
                        ControlSetStr (c, value);
                        break;
                    case V4L2_CTRL_TYPE_BITMASK:
                    {
                        unsigned long val = strtoul (value, &end, 0);
                        if (*end)
                        {
                            msg_Err (obj, "syntax error in \"%s\": "
                                     " not an integer", value);
                            goto next;
                        }
                        ControlSet (c, val);
                        break;
                    }
                    default:
                        msg_Err (obj, "setting \"%s\" not supported", name);
                        goto next;
                }
        msg_Err (obj, "control \"%s\" not available", name);
    }
    free (buf);
}
