}
void input_resource_TerminateVout( input_resource_t *p_resource )
{
    input_resource_RequestVout( p_resource, NULL, NULL, 0, false );
}
