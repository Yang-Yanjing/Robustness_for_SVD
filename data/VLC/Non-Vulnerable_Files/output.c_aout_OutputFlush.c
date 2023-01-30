 */
void aout_OutputFlush( audio_output_t *aout, bool wait )
{
    aout_OutputAssertLocked( aout );
    aout->flush (aout, wait);
}
