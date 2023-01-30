}
static void ChangePosition(intf_thread_t *intf, float increment)
{
    intf_sys_t     *sys = intf->p_sys;
    input_thread_t *p_input = sys->p_input;
    float pos;
    if (!p_input || var_GetInteger(p_input, "state") != PLAYING_S)
        return;
    pos = var_GetFloat(p_input, "position") + increment;
    if (pos > 0.99) pos = 0.99;
    if (pos < 0.0)  pos = 0.0;
    var_SetFloat(p_input, "position", pos);
}
