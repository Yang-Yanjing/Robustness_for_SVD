 */
char **vlc_sd_GetNames (vlc_object_t *obj, char ***pppsz_longnames, int **pp_categories)
{
    size_t count;
    vlc_sd_probe_t *tab = vlc_probe (obj, "services probe", &count);
    if (count == 0)
    {
        free (tab);
        return NULL;
    }
    char **names = malloc (sizeof(char *) * (count + 1));
    char **longnames = malloc (sizeof(char *) * (count + 1));
    int *categories = malloc(sizeof(int) * (count + 1));
    if (unlikely (names == NULL || longnames == NULL || categories == NULL))
        abort();
    for( size_t i = 0; i < count; i++ )
    {
        names[i] = tab[i].name;
        longnames[i] = tab[i].longname;
        categories[i] = tab[i].category;
    }
    free (tab);
    names[count] = longnames[count] = NULL;
    categories[count] = 0;
    *pppsz_longnames = longnames;
    if( pp_categories ) *pp_categories = categories;
    else free( categories );
    return names;
}
