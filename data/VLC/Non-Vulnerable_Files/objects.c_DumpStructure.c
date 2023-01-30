}
static void DumpStructure (vlc_object_internals_t *priv, unsigned i_level,
                           char *psz_foo)
{
    char i_back = psz_foo[i_level];
    psz_foo[i_level] = '\0';
    PrintObject (priv, psz_foo);
    psz_foo[i_level] = i_back;
    if( i_level / 2 >= MAX_DUMPSTRUCTURE_DEPTH )
    {
        msg_Warn( vlc_externals(priv), "structure tree is too deep" );
        return;
    }
    for (priv = priv->first; priv != NULL; priv = priv->next)
    {
        if( i_level )
        {
            psz_foo[i_level-1] = ' ';
            if( psz_foo[i_level-2] == '`' )
            {
                psz_foo[i_level-2] = ' ';
            }
        }
        psz_foo[i_level] = priv->next ? '|' : '`';
        psz_foo[i_level+1] = '-';
        psz_foo[i_level+2] = '\0';
        DumpStructure (priv, i_level + 2, psz_foo);
    }
}
