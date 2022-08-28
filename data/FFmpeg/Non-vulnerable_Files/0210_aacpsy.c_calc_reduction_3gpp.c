static float calc_reduction_3gpp(float a, float desired_pe, float pe,
                                 float active_lines)
{
    float thr_avg, reduction;
    if(active_lines == 0.0)
        return 0;
    thr_avg   = exp2f((a - pe) / (4.0f * active_lines));
    reduction = exp2f((a - desired_pe) / (4.0f * active_lines)) - thr_avg;
    return FFMAX(reduction, 0.0f);
}
