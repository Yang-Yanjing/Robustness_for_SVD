 ****************************************************************************/
static block_t *DoWork( filter_t *p_filter, block_t *p_in_buf )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    float *p_sample = (float *)p_in_buf->p_buffer;
    float i_value[AOUT_CHAN_MAX];
    int nbChannels = aout_FormatNbChannels(&p_filter->fmt_in.audio);
    for (int i = 0; i < nbChannels; i++)
        i_value[i] = 0.;
    /* 1 - Compute the peak values */
    float max = 0.0;
    for (size_t i = 0; i < p_in_buf->i_nb_samples; i++) {
        for (int j = 0; j<nbChannels; j++) {
            float ch = (*p_sample++);
            if (ch > i_value[j])
                i_value[j] = ch;
            if (ch > max)
                max = ch;
        }
    }
    max *= max;
    if (p_sys->silence) {
        /* 2 - store the new value */
        ValueDate_t *new = xmalloc(sizeof(*new));
        new->value = max;
        new->date = p_in_buf->i_pts;
        new->next = NULL;
        if (p_sys->last != NULL)
            p_sys->last->next = new;
        p_sys->last = new;
        if (p_sys->first == NULL)
            p_sys->first = new;
        /* 3 - delete too old values */
        while (p_sys->first->date < new->date - p_sys->time_window) {
            p_sys->started = 1; // we have enough values to compute a valid total
            ValueDate_t *current = p_sys->first;
            p_sys->first = p_sys->first->next;
            free(current);
        }
        /* If last message was sent enough time ago */
        if (p_sys->started && p_in_buf->i_pts > p_sys->lastAlarm + p_sys->repetition_time) {
            /* 4 - compute the RMS */
            ValueDate_t *current = p_sys->first;
            float sum = 0.0;
            int count = 0;
            while (current != NULL) {
                sum += current->value;
                count++;
                current = current->next;
            }
            sum /= count;
            sum = sqrtf(sum);
            /* 5 - compare it to the threshold */
            var_SetBool(p_filter->p_libvlc, "audiobargraph_v-alarm",
                        sum < p_sys->alarm_threshold);
            p_sys->lastAlarm = p_in_buf->i_pts;
        }
    }
    if (p_sys->bargraph && nbChannels > 0 && p_sys->counter++ > p_sys->bargraph_repetition) {
        SendValues(p_filter, i_value, nbChannels);
        p_sys->counter = 0;
    }
    return p_in_buf;
}
