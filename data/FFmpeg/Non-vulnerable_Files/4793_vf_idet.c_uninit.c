static av_cold void uninit(AVFilterContext *ctx)
{
    IDETContext *idet = ctx->priv;
    int level = strncmp(ctx->name, "auto-inserted", 13) ? AV_LOG_INFO : AV_LOG_DEBUG;
    av_log(ctx, level, "Repeated Fields: Neither:%6"PRId64" Top:%6"PRId64" Bottom:%6"PRId64"\n",
           idet->total_repeats[REPEAT_NONE],
           idet->total_repeats[REPEAT_TOP],
           idet->total_repeats[REPEAT_BOTTOM]
        );
    av_log(ctx, level, "Single frame detection: TFF:%6"PRId64" BFF:%6"PRId64" Progressive:%6"PRId64" Undetermined:%6"PRId64"\n",
           idet->total_prestat[TFF],
           idet->total_prestat[BFF],
           idet->total_prestat[PROGRESSIVE],
           idet->total_prestat[UNDETERMINED]
        );
    av_log(ctx, level, "Multi frame detection: TFF:%6"PRId64" BFF:%6"PRId64" Progressive:%6"PRId64" Undetermined:%6"PRId64"\n",
           idet->total_poststat[TFF],
           idet->total_poststat[BFF],
           idet->total_poststat[PROGRESSIVE],
           idet->total_poststat[UNDETERMINED]
        );
    av_frame_free(&idet->prev);
    av_frame_free(&idet->cur );
    av_frame_free(&idet->next);
}
