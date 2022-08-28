static int formats_declared(AVFilterContext *f)
{
    int i;
    for (i = 0; i < f->nb_inputs; i++) {
        if (!f->inputs[i]->out_formats)
            return 0;
        if (f->inputs[i]->type == AVMEDIA_TYPE_AUDIO &&
            !(f->inputs[i]->out_samplerates &&
              f->inputs[i]->out_channel_layouts))
            return 0;
    }
    for (i = 0; i < f->nb_outputs; i++) {
        if (!f->outputs[i]->in_formats)
            return 0;
        if (f->outputs[i]->type == AVMEDIA_TYPE_AUDIO &&
            !(f->outputs[i]->in_samplerates &&
              f->outputs[i]->in_channel_layouts))
            return 0;
    }
    return 1;
}
