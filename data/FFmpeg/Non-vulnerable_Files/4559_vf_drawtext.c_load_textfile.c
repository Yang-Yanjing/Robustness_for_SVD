static int load_textfile(AVFilterContext *ctx)
{
    DrawTextContext *s = ctx->priv;
    int err;
    uint8_t *textbuf;
    uint8_t *tmp;
    size_t textbuf_size;
    if ((err = av_file_map(s->textfile, &textbuf, &textbuf_size, 0, ctx)) < 0) {
        av_log(ctx, AV_LOG_ERROR,
               "The text file '%s' could not be read or is empty\n",
               s->textfile);
        return err;
    }
    if (textbuf_size > SIZE_MAX - 1 || !(tmp = av_realloc(s->text, textbuf_size + 1))) {
        av_file_unmap(textbuf, textbuf_size);
        return AVERROR(ENOMEM);
    }
    s->text = tmp;
    memcpy(s->text, textbuf, textbuf_size);
    s->text[textbuf_size] = 0;
    av_file_unmap(textbuf, textbuf_size);
    return 0;
}
