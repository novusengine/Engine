type ProximityTriggerFlag<Enum> =
{
    Name : "ProximityTriggerFlag";
    Type : u8;

    Fields :
    {
        None : Auto,
        IsServerAuthorative : Value<1>,
        IsServerSideOnly : Value<2>,
    };
}