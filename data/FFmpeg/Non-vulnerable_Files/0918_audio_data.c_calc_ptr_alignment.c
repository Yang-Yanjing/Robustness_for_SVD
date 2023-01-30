static void calc_ptr_alignment(AudioData *a)
{
    int p;
    int min_align = 128;
    for (p = 0; p < a->planes; p++) {
        int cur_align = 128;
        while ((intptr_t)a->data[p] % cur_align)
            cur_align >>= 1;
        if (cur_align < min_align)
            min_align = cur_align;
    }
    a->ptr_align = min_align;
}
