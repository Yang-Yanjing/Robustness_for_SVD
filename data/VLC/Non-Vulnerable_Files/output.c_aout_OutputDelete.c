 */
void aout_OutputDelete (audio_output_t *aout)
{
    aout_OutputAssertLocked (aout);
    var_DelCallback (aout, "stereo-mode", aout_ChannelsRestart, NULL);
    if (aout->stop != NULL)
        aout->stop (aout);
}
