static void copy_rev (FFTSample *dest, int w, int w2)
{
    int i;
    for (i = w; i < w + (w2-w)/2; i++)
        dest[i] = dest[2*w - i - 1];
    for (; i < w2; i++)
        dest[i] = dest[w2 - i];
}
