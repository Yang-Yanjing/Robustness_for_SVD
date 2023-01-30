}
void vout_control_WaitEmpty(vout_control_t *ctrl)
{
    vlc_mutex_lock(&ctrl->lock);
    while ((ctrl->cmd.i_size > 0 || ctrl->is_processing) && !ctrl->is_dead)
        vlc_cond_wait(&ctrl->wait_acknowledge, &ctrl->lock);
    vlc_mutex_unlock(&ctrl->lock);
}
