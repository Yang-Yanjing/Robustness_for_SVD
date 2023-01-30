}
void ControlsDeinit (vlc_object_t *obj, vlc_v4l2_ctrl_t *list)
{
    var_DelCallback (obj, "reset", ControlsResetCallback, list);
    var_Destroy (obj, "reset");
    while (list != NULL)
    {
        vlc_v4l2_ctrl_t *next = list->next;
        var_DelCallback (obj, list->name, ControlSetCallback, list);
        var_Destroy (obj, list->name);
        free (list);
        list = next;
    }
    var_Destroy (obj, "controls");
}
