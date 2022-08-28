static void lame_apply_block_type(AacPsyChannel *ctx, FFPsyWindowInfo *wi, int uselongblock)
{
    int blocktype = ONLY_LONG_SEQUENCE;
    if (uselongblock) {
        if (ctx->next_window_seq == EIGHT_SHORT_SEQUENCE)
            blocktype = LONG_STOP_SEQUENCE;
    } else {
        blocktype = EIGHT_SHORT_SEQUENCE;
        if (ctx->next_window_seq == ONLY_LONG_SEQUENCE)
            ctx->next_window_seq = LONG_START_SEQUENCE;
        if (ctx->next_window_seq == LONG_STOP_SEQUENCE)
            ctx->next_window_seq = EIGHT_SHORT_SEQUENCE;
    }
    wi->window_type[0] = ctx->next_window_seq;
    ctx->next_window_seq = blocktype;
}
