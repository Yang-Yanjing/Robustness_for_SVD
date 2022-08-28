static int parse_named_def(struct sbg_parser *p)
{
    char *cursor_save = p->cursor;
    struct sbg_string name;
    struct sbg_script_definition *def;
    if (!lex_name(p, &name) || !lex_char(p, ':') || !lex_space(p)) {
        p->cursor = cursor_save;
        return 0;
    }
    if (name.e - name.s == 6 && !memcmp(name.s, "wave", 4) &&
        name.s[4] >= '0' && name.s[4] <= '9' &&
        name.s[5] >= '0' && name.s[5] <= '9') {
        int wavenum = (name.s[4] - '0') * 10 + (name.s[5] - '0');
        return parse_wave_def(p, wavenum);
    }
    def = alloc_array_elem((void **)&p->scs.def, sizeof(*def),
                           &p->scs.nb_def, &p->nb_def_max);
    if (!def)
        return AVERROR(ENOMEM);
    def->name     = name.s;
    def->name_len = name.e - name.s;
    if (lex_char(p, '{'))
        return parse_block_def(p, def);
    return parse_synth_def(p, def);
}

