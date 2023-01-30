}
void vout_control_Wake(vout_control_t *ctrl)
{
    vlc_mutex_lock(&ctrl->lock);
    ctrl->can_sleep = false;
    vlc_cond_signal(&ctrl->wait_request);
    vlc_mutex_unlock(&ctrl->lock);
}
