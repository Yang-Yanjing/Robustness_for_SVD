void ff_ac3_output_frame(AC3EncodeContext *s, unsigned char *frame)
{
    int blk;
    init_put_bits(&s->pb, frame, AC3_MAX_CODED_FRAME_SIZE);
    s->output_frame_header(s);
    for (blk = 0; blk < s->num_blocks; blk++)
        output_audio_block(s, blk);
    output_frame_end(s);
}
