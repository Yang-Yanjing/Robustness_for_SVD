}
static void Eia608EraseScreen( eia608_t *h, bool b_displayed )
{
    Eia608ClearScreen( h, b_displayed ? h->i_screen : (1-h->i_screen) );
}
