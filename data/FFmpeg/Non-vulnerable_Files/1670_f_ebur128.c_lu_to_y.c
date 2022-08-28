static inline int lu_to_y(const EBUR128Context *ebur128, double v)
{
    v += 2 * ebur128->meter;                            
    v  = av_clipf(v, 0, ebur128->scale_range);          
    v  = ebur128->scale_range - v;                      
    return v * ebur128->graph.h / ebur128->scale_range; 
}
