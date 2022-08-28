static void set_metadata(AudioStatsContext *s, AVDictionary **metadata)
{
    uint64_t mask = 0, min_count = 0, max_count = 0, nb_samples = 0;
    double min_runs = 0, max_runs = 0,
           min = DBL_MAX, max = DBL_MIN, min_diff = DBL_MAX, max_diff = 0,
           max_sigma_x = 0,
           diff1_sum = 0,
           sigma_x = 0,
           sigma_x2 = 0,
           min_sigma_x2 = DBL_MAX,
           max_sigma_x2 = DBL_MIN;
    int c;
    for (c = 0; c < s->nb_channels; c++) {
        ChannelStats *p = &s->chstats[c];
        if (p->nb_samples < s->tc_samples)
            p->min_sigma_x2 = p->max_sigma_x2 = p->sigma_x2 / p->nb_samples;
        min = FFMIN(min, p->min);
        max = FFMAX(max, p->max);
        min_diff = FFMIN(min_diff, p->min_diff);
        max_diff = FFMAX(max_diff, p->max_diff);
        diff1_sum += p->diff1_sum,
        min_sigma_x2 = FFMIN(min_sigma_x2, p->min_sigma_x2);
        max_sigma_x2 = FFMAX(max_sigma_x2, p->max_sigma_x2);
        sigma_x += p->sigma_x;
        sigma_x2 += p->sigma_x2;
        min_count += p->min_count;
        max_count += p->max_count;
        min_runs += p->min_runs;
        max_runs += p->max_runs;
        mask |= p->mask;
        nb_samples += p->nb_samples;
        if (fabs(p->sigma_x) > fabs(max_sigma_x))
            max_sigma_x = p->sigma_x;
        set_meta(metadata, c + 1, "DC_offset", "%f", p->sigma_x / p->nb_samples);
        set_meta(metadata, c + 1, "Min_level", "%f", p->min);
        set_meta(metadata, c + 1, "Max_level", "%f", p->max);
        set_meta(metadata, c + 1, "Min_difference", "%f", p->min_diff);
        set_meta(metadata, c + 1, "Max_difference", "%f", p->max_diff);
        set_meta(metadata, c + 1, "Mean_difference", "%f", p->diff1_sum / (p->nb_samples - 1));
        set_meta(metadata, c + 1, "Peak_level", "%f", LINEAR_TO_DB(FFMAX(-p->min, p->max)));
        set_meta(metadata, c + 1, "RMS_level", "%f", LINEAR_TO_DB(sqrt(p->sigma_x2 / p->nb_samples)));
        set_meta(metadata, c + 1, "RMS_peak", "%f", LINEAR_TO_DB(sqrt(p->max_sigma_x2)));
        set_meta(metadata, c + 1, "RMS_trough", "%f", LINEAR_TO_DB(sqrt(p->min_sigma_x2)));
        set_meta(metadata, c + 1, "Crest_factor", "%f", p->sigma_x2 ? FFMAX(-p->min, p->max) / sqrt(p->sigma_x2 / p->nb_samples) : 1);
        set_meta(metadata, c + 1, "Flat_factor", "%f", LINEAR_TO_DB((p->min_runs + p->max_runs) / (p->min_count + p->max_count)));
        set_meta(metadata, c + 1, "Peak_count", "%f", (float)(p->min_count + p->max_count));
        set_meta(metadata, c + 1, "Bit_depth", "%f", bit_depth(p->mask));
    }
    set_meta(metadata, 0, "Overall.DC_offset", "%f", max_sigma_x / (nb_samples / s->nb_channels));
    set_meta(metadata, 0, "Overall.Min_level", "%f", min);
    set_meta(metadata, 0, "Overall.Max_level", "%f", max);
    set_meta(metadata, 0, "Overall.Min_difference", "%f", min_diff);
    set_meta(metadata, 0, "Overall.Max_difference", "%f", max_diff);
    set_meta(metadata, 0, "Overall.Mean_difference", "%f", diff1_sum / (nb_samples - s->nb_channels));
    set_meta(metadata, 0, "Overall.Peak_level", "%f", LINEAR_TO_DB(FFMAX(-min, max)));
    set_meta(metadata, 0, "Overall.RMS_level", "%f", LINEAR_TO_DB(sqrt(sigma_x2 / nb_samples)));
    set_meta(metadata, 0, "Overall.RMS_peak", "%f", LINEAR_TO_DB(sqrt(max_sigma_x2)));
    set_meta(metadata, 0, "Overall.RMS_trough", "%f", LINEAR_TO_DB(sqrt(min_sigma_x2)));
    set_meta(metadata, 0, "Overall.Flat_factor", "%f", LINEAR_TO_DB((min_runs + max_runs) / (min_count + max_count)));
    set_meta(metadata, 0, "Overall.Peak_count", "%f", (float)(min_count + max_count) / (double)s->nb_channels);
    set_meta(metadata, 0, "Overall.Bit_depth", "%f", bit_depth(mask));
    set_meta(metadata, 0, "Overall.Number_of_samples", "%f", nb_samples / s->nb_channels);
}
