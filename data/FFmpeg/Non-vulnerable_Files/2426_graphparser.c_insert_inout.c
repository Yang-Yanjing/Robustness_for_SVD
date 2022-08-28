static void insert_inout(AVFilterInOut **inouts, AVFilterInOut *element)
{
    element->next = *inouts;
    *inouts = element;
}
