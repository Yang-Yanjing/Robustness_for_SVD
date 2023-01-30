}
void input_resource_Terminate( input_resource_t *p_resource )
{
    input_resource_TerminateSout( p_resource );
    input_resource_ResetAout( p_resource );
    input_resource_TerminateVout( p_resource );
}
