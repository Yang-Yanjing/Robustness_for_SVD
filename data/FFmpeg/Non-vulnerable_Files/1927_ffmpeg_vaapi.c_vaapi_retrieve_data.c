static int vaapi_retrieve_data(AVCodecContext *avctx, AVFrame *input)
{
    InputStream *ist = avctx->opaque;
    VAAPIDecoderContext *ctx = ist->hwaccel_ctx;
    AVFrame *output = 0;
    int err;
    av_assert0(input->format == AV_PIX_FMT_VAAPI);
    if (ctx->output_format == AV_PIX_FMT_VAAPI) {
        
        return 0;
    }
    av_log(ctx, AV_LOG_DEBUG, "Retrieve data from surface %#x.\n",
           (unsigned int)(uintptr_t)input->data[3]);
    output = av_frame_alloc();
    if (!output)
        return AVERROR(ENOMEM);
    output->format = ctx->output_format;
    err = av_hwframe_transfer_data(output, input, 0);
    if (err < 0) {
        av_log(ctx, AV_LOG_ERROR, "Failed to transfer data to "
               "output frame: %d.\n", err);
        goto fail;
    }
    err = av_frame_copy_props(output, input);
    if (err < 0) {
        av_frame_unref(output);
        goto fail;
    }
    av_frame_unref(input);
    av_frame_move_ref(input, output);
    av_frame_free(&output);
    return 0;
fail:
    if (output)
        av_frame_free(&output);
    return err;
}
