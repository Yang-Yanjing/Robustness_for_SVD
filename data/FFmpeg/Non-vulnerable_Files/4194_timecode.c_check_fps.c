static int check_fps(int fps)
{
    int i;
    static const int supported_fps[] = {24, 25, 30, 48, 50, 60};
    for (i = 0; i < FF_ARRAY_ELEMS(supported_fps); i++)
        if (fps == supported_fps[i])
            return 0;
    return -1;
}
