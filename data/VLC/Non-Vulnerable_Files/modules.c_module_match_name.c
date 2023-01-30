}
static bool module_match_name (const module_t *m, const char *name)
{
     /* Plugins with zero score must be matched explicitly. */
     if (!strcasecmp ("any", name))
         return m->i_score > 0;
     for (unsigned i = 0; i < m->i_shortcuts; i++)
          if (!strcasecmp (m->pp_shortcuts[i], name))
              return true;
     return false;
}
