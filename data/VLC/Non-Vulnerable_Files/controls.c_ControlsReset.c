}
static void ControlsReset (vlc_object_t *obj, vlc_v4l2_ctrl_t *list)
{
    while (list != NULL)
    {
        switch (list->type)
        {
            case V4L2_CTRL_TYPE_INTEGER:
            case V4L2_CTRL_TYPE_MENU:
            case V4L2_CTRL_TYPE_INTEGER_MENU:
                var_SetInteger (obj, list->name, list->default_value);
                break;
            case V4L2_CTRL_TYPE_BOOLEAN:
                var_SetBool (obj, list->name, list->default_value);
                break;
            default:;
        }
        list = list->next;
    }
}
