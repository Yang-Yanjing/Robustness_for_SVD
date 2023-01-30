}
static void MorphToPM( void )
{
    PPIB pib;
    DosGetInfoBlocks(NULL, &pib);
    /* Change flag from VIO to PM */
    if (pib->pib_ultype == 2)
        pib->pib_ultype = 3;
}
