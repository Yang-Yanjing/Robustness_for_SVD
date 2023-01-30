static float frand(int x, int y)
{
    const float r = sinf(x * 12.9898 + y * 78.233) * 43758.545;
    return r - floorf(r);
}
