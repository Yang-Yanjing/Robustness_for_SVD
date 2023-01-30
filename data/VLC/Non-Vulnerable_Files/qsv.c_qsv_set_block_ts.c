 */
static void qsv_set_block_ts(encoder_t *enc, encoder_sys_t *sys, block_t *block, mfxBitstream *bs)
{
    if (!bs->TimeStamp)
        return;
    block->i_pts = qsv_timestamp_to_mtime(bs->TimeStamp) + sys->offset_pts;
    block->i_dts = qsv_timestamp_to_mtime(bs->DecodeTimeStamp) + sys->offset_pts;
    /* HW encoder (with old driver versions) and some parameters
       combinations doesn't set the DecodeTimeStamp field so we warn
       the user about it */
    if (!bs->DecodeTimeStamp || bs->DecodeTimeStamp > (int64_t)bs->TimeStamp)
        if (sys->dts_warn_counter++ % 16 == 0) // Rate limiting this message.
            msg_Warn(enc, "Encode returning empty DTS or DTS > PTS. Your stream will be invalid. "
                     " Please double-check they weren't any warning at encoder initialization "
                     " and that you have the last version of Intel's drivers installed.");
}
