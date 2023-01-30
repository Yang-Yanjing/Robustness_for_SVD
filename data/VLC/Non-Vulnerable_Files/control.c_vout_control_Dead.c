}
void vout_control_Dead(vout_control_t *ctrl)
{
    vlc_mutex_lock(&ctrl->lock);
    ctrl->is_dead = true;
    vlc_cond_broadcast(&ctrl->wait_acknowledge);
    vlc_mutex_unlock(&ctrl->lock);
}
