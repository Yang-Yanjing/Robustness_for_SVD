static int config_input(AVFilterLink *inlink)
{
    RemoveGrainContext *s = inlink->dst->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    int i;
    s->nb_planes = av_pix_fmt_count_planes(inlink->format);
    s->planeheight[1] = s->planeheight[2] = FF_CEIL_RSHIFT(inlink->h, desc->log2_chroma_h);
    s->planeheight[0] = s->planeheight[3] = inlink->h;
    s->planewidth[1]  = s->planewidth[2]  = FF_CEIL_RSHIFT(inlink->w, desc->log2_chroma_w);
    s->planewidth[0]  = s->planewidth[3]  = inlink->w;
    for (i = 0; i < s->nb_planes; i++) {
        switch (s->mode[i]) {
        case 1:  s->rg[i] = mode01;   break;
        case 2:  s->rg[i] = mode02;   break;
        case 3:  s->rg[i] = mode03;   break;
        case 4:  s->rg[i] = mode04;   break;
        case 5:  s->rg[i] = mode05;   break;
        case 6:  s->rg[i] = mode06;   break;
        case 7:  s->rg[i] = mode07;   break;
        case 8:  s->rg[i] = mode08;   break;
        case 9:  s->rg[i] = mode09;   break;
        case 10: s->rg[i] = mode10;   break;
        case 11: s->rg[i] = mode1112; break;
        case 12: s->rg[i] = mode1112; break;
        case 13: s->skip_odd = 1;
                 s->rg[i] = mode1314; break;
        case 14: s->skip_even = 1;
                 s->rg[i] = mode1314; break;
        case 15: s->skip_odd = 1;
                 s->rg[i] = mode1516; break;
        case 16: s->skip_even = 1;
                 s->rg[i] = mode1516; break;
        case 17: s->rg[i] = mode17;   break;
        case 18: s->rg[i] = mode18;   break;
        case 19: s->rg[i] = mode19;   break;
        case 20: s->rg[i] = mode20;   break;
        case 21: s->rg[i] = mode21;   break;
        case 22: s->rg[i] = mode22;   break;
        case 23: s->rg[i] = mode23;   break;
        case 24: s->rg[i] = mode24;   break;
        }
    }
    if (ARCH_X86)
        ff_removegrain_init_x86(s);
    return 0;
}
