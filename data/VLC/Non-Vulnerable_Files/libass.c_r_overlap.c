}
static bool r_overlap( const rectangle_t *a, const rectangle_t *b, int i_dx, int i_dy )
{
    return  __MAX(a->x0-i_dx, b->x0) < __MIN( a->x1+i_dx, b->x1 ) &&
            __MAX(a->y0-i_dy, b->y0) < __MIN( a->y1+i_dy, b->y1 );
}
