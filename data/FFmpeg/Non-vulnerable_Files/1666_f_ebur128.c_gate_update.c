static int gate_update(struct integrator *integ, double power,
                       double loudness, int gate_thres)
{
    int ipower;
    double relative_threshold;
    int gate_hist_pos;
    
    ipower = av_clip(HIST_POS(loudness), 0, HIST_SIZE - 1);
    integ->histogram[ipower].count++;
    
    integ->sum_kept_powers += power;
    integ->nb_kept_powers++;
    relative_threshold = integ->sum_kept_powers / integ->nb_kept_powers;
    if (!relative_threshold)
        relative_threshold = 1e-12;
    integ->rel_threshold = LOUDNESS(relative_threshold) + gate_thres;
    gate_hist_pos = av_clip(HIST_POS(integ->rel_threshold), 0, HIST_SIZE - 1);
    return gate_hist_pos;
}
