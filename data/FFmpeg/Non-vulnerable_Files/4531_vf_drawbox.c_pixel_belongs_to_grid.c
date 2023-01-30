static av_pure av_always_inline int pixel_belongs_to_grid(DrawBoxContext *drawgrid, int x, int y)
{
    
    
    int x_modulo;
    int y_modulo;
    
    x -= drawgrid->x;
    y -= drawgrid->y;
    x_modulo = x % drawgrid->w;
    y_modulo = y % drawgrid->h;
    
    if (x_modulo < 0)
        x_modulo += drawgrid->w;
    if (y_modulo < 0)
        y_modulo += drawgrid->h;
    return x_modulo < drawgrid->thickness  
        || y_modulo < drawgrid->thickness;  
}
