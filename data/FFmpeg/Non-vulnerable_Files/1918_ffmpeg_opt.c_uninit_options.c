static int copy_unknown_streams = 0;
static void uninit_options(OptionsContext *o)
{
    const OptionDef *po = options;
    int i;
    
    while (po->name) {
        void *dst = (uint8_t*)o + po->u.off;
        if (po->flags & OPT_SPEC) {
            SpecifierOpt **so = dst;
            int i, *count = (int*)(so + 1);
            for (i = 0; i < *count; i++) {
                av_freep(&(*so)[i].specifier);
                if (po->flags & OPT_STRING)
                    av_freep(&(*so)[i].u.str);
            }
            av_freep(so);
            *count = 0;
        } else if (po->flags & OPT_OFFSET && po->flags & OPT_STRING)
            av_freep(dst);
        po++;
    }
    for (i = 0; i < o->nb_stream_maps; i++)
        av_freep(&o->stream_maps[i].linklabel);
    av_freep(&o->stream_maps);
    av_freep(&o->audio_channel_maps);
    av_freep(&o->streamid_map);
    av_freep(&o->attachments);
}
