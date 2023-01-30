};
static bool DeinterlaceIsModeValid(const char *mode)
{
    for (unsigned i = 0; i < ARRAY_SIZE(deinterlace_modes); i++) {
        if (!strcmp(deinterlace_modes[i], mode))
            return true;
    }
    return false;
}
