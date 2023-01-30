}
static void r_add( rectangle_t *r, const rectangle_t *n )
{
    r->x0 = __MIN( r->x0, n->x0 );
    r->y0 = __MIN( r->y0, n->y0 );
    r->x1 = __MAX( r->x1, n->x1 );
    r->y1 = __MAX( r->y1, n->y1 );
}
