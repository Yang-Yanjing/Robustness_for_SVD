 */
void aout_OutputPlay (audio_output_t *aout, block_t *block)
{
    aout_OutputAssertLocked (aout);
    aout->play (aout, block);
}
