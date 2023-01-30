} rar_pattern_t;
static const rar_pattern_t *FindVolumePattern(const char *location, bool b_extonly )
{
    static const rar_pattern_t patterns[] = {
        { ".part01.rar",  "%s.part%.2d.rar", 2,  99, false }, // new naming
        { ".part001.rar", "%s.part%.3d.rar", 2, 999, false }, // new
        { ".rar",         "%s.%c%.2d",       0, 999, true },  // old
        { NULL, NULL, 0, 0, false },
    };
    const size_t location_size = strlen(location);
    for (int i = 0; patterns[i].match != NULL; i++) {
        const size_t match_size = strlen(patterns[i].match);
        if (location_size < match_size)
            continue;
        if ( b_extonly && !patterns[i].b_extonly )
            continue;
        if (!strcmp(&location[location_size - match_size], patterns[i].match))
            return &patterns[i];
    }
    return NULL;
}
