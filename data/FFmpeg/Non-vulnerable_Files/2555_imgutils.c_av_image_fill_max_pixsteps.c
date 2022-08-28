void av_image_fill_max_pixsteps(int max_pixsteps[4], int max_pixstep_comps[4],
                                const AVPixFmtDescriptor *pixdesc)
{
    int i;
    memset(max_pixsteps, 0, 4*sizeof(max_pixsteps[0]));
    if (max_pixstep_comps)
        memset(max_pixstep_comps, 0, 4*sizeof(max_pixstep_comps[0]));
    for (i = 0; i < 4; i++) {
        const AVComponentDescriptor *comp = &(pixdesc->comp[i]);
        if ((comp->step_minus1+1) > max_pixsteps[comp->plane]) {
            max_pixsteps[comp->plane] = comp->step_minus1+1;
            if (max_pixstep_comps)
                max_pixstep_comps[comp->plane] = i;
        }
    }
}
