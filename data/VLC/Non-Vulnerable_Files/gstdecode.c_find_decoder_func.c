/* Check if the element can use this caps */
static gint find_decoder_func( gconstpointer p_p1, gconstpointer p_p2 )
{
    GstElementFactory *p_factory;
    sink_src_caps_t *p_caps;
    p_factory = ( GstElementFactory* )p_p1;
    p_caps = ( sink_src_caps_t* )p_p2;
    return !( gst_element_factory_can_sink_any_caps( p_factory,
                p_caps->p_sinkcaps ) &&
            gst_element_factory_can_src_any_caps( p_factory,
                p_caps->p_srccaps ) );
}
