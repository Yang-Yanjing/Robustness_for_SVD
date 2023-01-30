}
static void StreamIdRelease( bool *id, int i_id_min, int i_id )
{
    id[i_id - i_id_min] = true;
}
