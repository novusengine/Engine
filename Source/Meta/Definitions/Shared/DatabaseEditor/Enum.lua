local Type = require("Type")
local D = require("Definition")

return D.Definitions
{
    D.Enum("DatabaseEditorTypeEnum", Type.U8,
    {
        D.Field("Spell"),
        D.Field("Map"),
        D.Field("Interaction"),
        D.Field("Count")
    }),

    D.Enum("DatabaseEditorActionEnum", Type.U8,
    {
        D.Field("Snapshot"),
        D.Field("Mutation"),
        D.Field("Count")
    }),

    D.Enum("DatabaseEditorMutationTypeEnum", Type.U8,
    {
        D.Field("Create"),
        D.Field("Update"),
        D.Field("Delete"),
        D.Field("Count")
    }),

    D.Enum("DatabaseEditorTransferPhaseEnum", Type.U8,
    {
        D.Field("Begin"),
        D.Field("Chunk"),
        D.Field("Commit"),
        D.Field("Count")
    })
}
