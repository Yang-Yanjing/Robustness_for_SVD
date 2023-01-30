}
static int PushFont(font_stack_t **p_font, const char *psz_name, int i_size,
                     uint32_t i_color)
{
    font_stack_t *p_new;
    if (!p_font)
        return VLC_EGENERIC;
    p_new = malloc(sizeof(font_stack_t));
    if (! p_new)
        return VLC_ENOMEM;
    p_new->p_next = NULL;
    if (psz_name)
        p_new->psz_name = strdup(psz_name);
    else
        p_new->psz_name = NULL;
    p_new->i_size   = i_size;
    p_new->i_color  = i_color;
    if (!*p_font)
        *p_font = p_new;
    else {
        font_stack_t *p_last;
        for (p_last = *p_font; p_last->p_next; p_last = p_last->p_next)
        ;
        p_last->p_next = p_new;
    }
    return VLC_SUCCESS;
}
