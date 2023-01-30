static int config_props(AVFilterLink *link)
{
    DeshakeContext *deshake = link->dst->priv;
    deshake->ref = NULL;
    deshake->last.vec.x = 0;
    deshake->last.vec.y = 0;
    deshake->last.angle = 0;
    deshake->last.zoom = 0;
    return 0;
}
