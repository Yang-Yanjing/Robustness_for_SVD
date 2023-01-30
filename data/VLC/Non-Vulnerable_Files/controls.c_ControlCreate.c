 */
static vlc_v4l2_ctrl_t *ControlCreate (int fd,
                                       const struct v4l2_queryctrl *query)
{
    vlc_v4l2_ctrl_t *ctrl = malloc (sizeof (*ctrl));
    if (unlikely(ctrl == NULL))
        return NULL;
    ctrl->fd = fd;
    ctrl->id = query->id;
    ctrl->type = query->type;
    /* Search for a well-known control */
    const vlc_v4l2_ctrl_name_t *known;
    known = bsearch (&query->id, controls, sizeof (controls) / sizeof (*known),
                     sizeof (*known), cidcmp);
    if (known != NULL)
        strcpy (ctrl->name, known->name);
    else
    /* Fallback to automatically-generated control name */
    {
        size_t i;
        for (i = 0; query->name[i]; i++)
        {
            unsigned char c = query->name[i];
            if (c == ' ' || c == ',')
                c = '_';
            if (c < 128)
                c = tolower (c);
            ctrl->name[i] = c;
        }
        ctrl->name[i] = '\0';
    }
    ctrl->default_value = query->default_value;
    return ctrl;
}
