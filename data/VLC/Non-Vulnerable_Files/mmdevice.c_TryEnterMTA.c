   0xad, 0xb2, 0xc6, 0x8b, 0x50, 0x1a, 0x66, 0x55);
static int TryEnterMTA(vlc_object_t *obj)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (unlikely(FAILED(hr)))
    {
        msg_Err (obj, "cannot initialize COM (error 0x%lx)", hr);
        return -1;
    }
    return 0;
}
