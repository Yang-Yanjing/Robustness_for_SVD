#define TryEnterMTA(o) TryEnterMTA(VLC_OBJECT(o))
static void EnterMTA(void)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (unlikely(FAILED(hr)))
        abort();
}
