void uninit_parse_context(OptionParseContext *octx)
{
    int i, j;
    for (i = 0; i < octx->nb_groups; i++) {
        OptionGroupList *l = &octx->groups[i];
        for (j = 0; j < l->nb_groups; j++) {
            av_freep(&l->groups[j].opts);
            av_dict_free(&l->groups[j].codec_opts);
            av_dict_free(&l->groups[j].format_opts);
            av_dict_free(&l->groups[j].resample_opts);
            av_dict_free(&l->groups[j].sws_dict);
            av_dict_free(&l->groups[j].swr_opts);
        }
        av_freep(&l->groups);
    }
    av_freep(&octx->groups);
    av_freep(&octx->cur_group.opts);
    av_freep(&octx->global_opts.opts);
    uninit_opts();
}
