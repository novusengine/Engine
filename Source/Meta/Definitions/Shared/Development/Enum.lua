local Type = require("Type")
local D = require("Definition")

return D.Definitions
{
    D.Enum("DevelopmentResourceEnum", Type.U8,
    {
        D.Field("None"),
        D.Field("CreatureAIScript"),
        D.Field("ScriptWorkspace")
    }),

    D.Enum("DevelopmentOperationEnum", Type.U8,
    {
        D.Field("None"),
        D.Field("Checkout"),
        D.Field("Reserve"),
        D.Field("RenewLease"),
        D.Field("ReleaseLease"),
        D.Field("Upload"),
        D.Field("Catalog"),
        D.Field("Fetch"),
        D.Field("Inspect"),
        D.Field("Link"),
        D.Field("Unlink")
    }),

    D.Enum("DevelopmentResultEnum", Type.U8,
    {
        D.Field("Success"),
        D.Field("InvalidRequest"),
        D.Field("PermissionDenied"),
        D.Field("NotFound"),
        D.Field("AlreadyExists"),
        D.Field("Locked"),
        D.Field("LeaseRejected"),
        D.Field("TransferFailed"),
        D.Field("RevisionConflict")
    }),

    D.Enum("CreatureAIScriptBindingScopeEnum", Type.U8,
    {
        D.Field("None"),
        D.Field("Guid"),
        D.Field("Template")
    })
}
