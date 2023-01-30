}
static void Unlock(void *opaque, uint8_t *data)
{
    assert((uintptr_t)opaque == OPAQUE_MAGIC);
    assert((uintptr_t)data == DATA_MAGIC);
}
