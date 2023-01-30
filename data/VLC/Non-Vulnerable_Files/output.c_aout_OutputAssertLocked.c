/* Local functions */
static void aout_OutputAssertLocked (audio_output_t *aout)
{
    aout_owner_t *owner = aout_owner (aout);
    vlc_assert_locked (&owner->lock);
}
