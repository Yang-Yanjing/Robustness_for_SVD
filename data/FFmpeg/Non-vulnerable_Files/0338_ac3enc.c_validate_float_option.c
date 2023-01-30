static int validate_float_option(float v, const float *v_list, int v_list_size)
{
    int i;
    for (i = 0; i < v_list_size; i++) {
        if (v < (v_list[i] + FLT_OPTION_THRESHOLD) &&
            v > (v_list[i] - FLT_OPTION_THRESHOLD))
            break;
    }
    if (i == v_list_size)
        return -1;
    return i;
}
