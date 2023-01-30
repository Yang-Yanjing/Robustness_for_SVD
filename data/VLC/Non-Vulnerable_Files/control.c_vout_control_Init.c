/* */
void vout_control_Init(vout_control_t *ctrl)
{
    vlc_mutex_init(&ctrl->lock);
    vlc_cond_init(&ctrl->wait_request);
    vlc_cond_init(&ctrl->wait_acknowledge);
    ctrl->is_dead = false;
    ctrl->can_sleep = true;
    ctrl->is_processing = false;
    ARRAY_INIT(ctrl->cmd);
}
