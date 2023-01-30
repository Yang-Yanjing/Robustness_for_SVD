 */
static void qsv_set_block_flags(block_t *block, uint16_t frame_type)
{
    if ((frame_type & MFX_FRAMETYPE_IDR) || (frame_type & MFX_FRAMETYPE_REF))
        block->i_flags = BLOCK_FLAG_TYPE_I;
    else if ((frame_type & MFX_FRAMETYPE_P) || (frame_type & MFX_FRAMETYPE_I))
        block->i_flags = BLOCK_FLAG_TYPE_P;
    else if (frame_type & MFX_FRAMETYPE_B)
        block->i_flags = BLOCK_FLAG_TYPE_B;
    else
        block->i_flags = BLOCK_FLAG_TYPE_PB;
}
