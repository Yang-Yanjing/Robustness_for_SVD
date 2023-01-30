}
static HINSTANCE Direct3DLoadShaderLibrary(void)
{
    HINSTANCE instance = NULL;
    for (int i = 43; i > 23; --i) {
        TCHAR filename[16];
        _sntprintf(filename, 16, TEXT("D3dx9_%d.dll"), i);
        instance = LoadLibrary(filename);
        if (instance)
            break;
    }
    return instance;
}
