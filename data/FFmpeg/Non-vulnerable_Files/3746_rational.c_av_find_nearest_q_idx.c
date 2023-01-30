int av_find_nearest_q_idx(AVRational q, const AVRational* q_list)
{
    int i, nearest_q_idx = 0;
    for (i = 0; q_list[i].den; i++)
        if (av_nearer_q(q, q_list[i], q_list[nearest_q_idx]) > 0)
            nearest_q_idx = i;
    return nearest_q_idx;
}
