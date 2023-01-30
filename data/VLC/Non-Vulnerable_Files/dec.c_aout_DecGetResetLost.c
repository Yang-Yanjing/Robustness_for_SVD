}
int aout_DecGetResetLost (audio_output_t *aout)
{
    aout_owner_t *owner = aout_owner (aout);
    return atomic_exchange(&owner->buffers_lost, 0);
}
