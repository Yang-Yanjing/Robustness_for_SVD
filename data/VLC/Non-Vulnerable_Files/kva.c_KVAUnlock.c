}
static void KVAUnlock( picture_t *picture )
{
    VLC_UNUSED( picture );
    kvaUnlockBuffer();
}
