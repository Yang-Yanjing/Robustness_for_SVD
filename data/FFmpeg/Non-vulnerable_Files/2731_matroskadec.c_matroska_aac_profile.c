static int matroska_aac_profile(char *codec_id)
{
    static const char *const aac_profiles[] = { "MAIN", "LC", "SSR" };
    int profile;
    for (profile = 0; profile < FF_ARRAY_ELEMS(aac_profiles); profile++)
        if (strstr(codec_id, aac_profiles[profile]))
            break;
    return profile + 1;
}
