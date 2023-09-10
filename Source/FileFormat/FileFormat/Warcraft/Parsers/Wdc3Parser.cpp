#include "Wdc3Parser.h"
#include "FileFormat/Shared.h"
#include "FileFormat/Warcraft/DB2/Wdc3.h"

#include <Base/Memory/Bytebuffer.h>

using namespace DB2::WDC3;

robin_hood::unordered_set<u64> Parser::_cascEncryptionKeyLookup =
{
	// Battle.net app
	0x2C547F26A2613E01ULL,

	// Starcraft
	0xD0CAE11366CEEA83ULL,

	// Warcraft III Reforge,
	0x6E4296823E7D561EULL,
	0xE04D60E31DDEBF63ULL,

	// Overwatch
	0xFB680CB6A8BF81F3ULL,
	0x402CD9D8D6BFED98ULL,
	0xDBD3371554F60306ULL,
	0x11A9203C9881710AULL,
	0xA19C4F859F6EFA54ULL,
	0x87AEBBC9C4E6B601ULL,
	0xDEE3A0521EFF6F03ULL,
	0x8C9106108AA84F07ULL,
	0x49166D358A34D815ULL,
	0x1463A87356778D14ULL,
	0x5E152DE44DFBEE01ULL,
	0x9B1F39EE592CA415ULL,
	0x24C8B75890AD5917ULL,
	0xEA658B75FDD4890FULL,
	0x026FDCDF8C5C7105ULL,
	0xCAE3FAC925F20402ULL,
	0x061581CA8496C80CULL,
	0xBE2CB0FAD3698123ULL,
	0x57A5A33B226B8E0AULL,
	0x42B9AB1AF5015920ULL,
	0x4F0FE18E9FA1AC1AULL,
	0x7758B2CF1E4E3E1BULL,
	0xE5317801B3561125ULL,
	0x16B866D7BA3A8036ULL,
	0x11131FFDA0D18D30ULL,
	0xCAC6B95B2724144AULL,
	0xB7DBC693758A5C36ULL,
	0x90CA73B2CDE3164BULL,
	0x6DD3212FB942714AULL,
	0x11DDB470ABCBA130ULL,
	0x5BEF27EEE95E0B4BULL,
	0x9359B46E49D2DA42ULL,
	0x1A46302EF8896F34ULL,
	0x693529F7D40A064CULL,
	0x388B85AEEDCB685DULL,
	0xE218F69AAC6C104DULL,
	0xF432F0425363F250ULL,
	0x061D52F86830B35DULL,
	0x1275C84CF113EF65ULL,
	0xD9C7C7AC0F14C868ULL,
	0xBD4E42661A432951ULL,
	0xC43CB14355249451ULL,
	0xE6D914F8E4744953ULL,
	0x5694C503F8C80178ULL,
	0x21DBFD65F3E54269ULL,
	0x27ABA5F88DD8D078ULL,
	0x21E1F90E71D33C71ULL,
	0xD9CB055BCDD40B6EULL,
	0x8175CE3C694C6659ULL,
	0xB8DE51690075435AULL,
	0xF6CF23955B5D437DULL,
	0x0E4D9426F2891F5CULL,
	0x9240BA6A2A0CF684ULL,
	0x82297FBAB7F5EB80ULL,
	0x9ADF00AA1A174A69ULL,
	0xCFA05AA76B49F881ULL,
	0x493455579DA0B18EULL,
	0x6362C5AD65DAE686ULL,
	0x8162E5313A9C135DULL,
	0x68EAE8FDC008C381ULL,
	0xF412C6327C4BF091ULL,
	0x3B3ED0874091B174ULL,
	0x37FD04E05D2A6292ULL,
	0xC0DDC77552BE5794ULL,
	0x68D8EB839DC15D75ULL,
	0x209F33BBAC9D1295ULL,
	0xA55F8C6F20454D94ULL,
	0x3EEEDB8E7C29A09BULL,
	0x22C1AF6758F8449EULL,
	0x11B2E01B9331799EULL,
	0x8498337C740329B3ULL,
	0xE6E8BCABE3CC96C1ULL,
	0x5D4AC0DC6F3113BAULL,
	0x27F9D85973DCD5AFULL,
	0x67AAD845CC0F03BDULL,
	0xCA13F0C79042A1A0ULL,
	0xC4D84093A32684BDULL,
	0x0BFE5A2B3C606BA1ULL,
	0x402CD9D8D6BFED98ULL,
	0xF1CBDF48147D26C6ULL,
	0x01A48CFAE25F85CDULL,
	0xFEBBF66DAEF6C9BEULL,
	0x2AD4834DAB3986ABULL,
	0xD89B24D62F00A04EULL,
	0x32DDC40236DAEA7BULL,
	0xF481EFC2302EE415ULL,
	0xD1AC8C1903524D9AULL,
	0x71EEBE93590AA903ULL,

	// Streamed WoW keys
	0xFA505078126ACB3EULL,
	0xFF813F7D062AC0BCULL,
	0xD1E9B5EDF9283668ULL,
	0xB76729641141CB34ULL,
	0xFFB9469FF16E6BF8ULL,
	0x23C5B5DF837A226CULL,
	0x3AE403EF40AC3037ULL,
	0xE2854509C471C554ULL,
	0x8EE2CB82178C995AULL,
	0x5813810F4EC9B005ULL,
	0x7F9E217166ED43EAULL,
	0xC4A8D364D23793F7ULL,
	0x40A234AEBCF2C6E5ULL,
	0x9CF7DFCFCBCE4AE5ULL,
	0x4E4BDECAB8485B4FULL,
	0x94A50AC54EFF70E4ULL,
	0xBA973B0E01DE1C2CULL,
	0x494A6F8E8E108BEFULL,
	0x918D6DD0C3849002ULL,
	0x0B5F6957915ADDCAULL,
	0x794F25C6CD8AB62BULL,
	0xA9633A54C1673D21ULL,
	0x5E5D896B3E163DEAULL,
	0x0EBE36B5010DFD7FULL,
	0x01E828CFFA450C0FULL,
	0x4A7BD170FE18E6AEULL,
	0x69549CB975E87C4FULL,
	0x460C92C372B2A166ULL,
	0x8165D801CCA11962ULL,
	0xA3F1C999090ADAC9ULL,
	0x18AFDF5191923610ULL,
	0x3C258426058FBD93ULL,
	0x094E9A0474876B98ULL,
	0x3DB25CB86A40335EULL,
	0x0DCD81945F4B4686ULL,
	0x486A2A3A2803BE89ULL,
	0x71F69446AD848E06ULL,
	0x211FCD1265A928E9ULL,
	0x0ADC9E327E42E98CULL,
	0xBAE9F621B60174F1ULL,
	0x34DE1EEADC97115EULL,
	0xE07E107F1390A3DFULL,
	0x32690BF74DE12530ULL,
	0xBF3734B1DCB04696ULL,
	0x74F4F78002A5A1BEULL,
	0x423F07656CA27D23ULL,
	0x0691678F83E8A75DULL,
	0x324498590F550556ULL,
	0xC02C78F40BEF5998ULL,
	0x47011412CCAAB541ULL,
	0x23B6F5764CE2DDD6ULL,
	0x8E00C6F405873583ULL,
	0x78482170E4CFD4A6ULL,
	0xB1EB52A64BFAF7BFULL,
	0xFC6F20EE98D208F6ULL,
	0x402CFABF2020D9B7ULL,
	0x6FA0420E902B4FBEULL,
	0x1076074F2B350A2DULL,
	0x816F00C1322CDF52ULL,
	0xDDD295C82E60DB3CULL,
	0x83E96F07F259F799ULL,
	0x49FBFE8A717F03D5ULL,
	0xC1E5D7408A7D4484ULL,
	0xE46276EB9E1A9854ULL,
	0xD245B671DD78648CULL,
	0x4C596E12D36DDFC3ULL,
	0x0C9ABD5081C06411ULL,
	0x3C6243057F3D9B24ULL,
	0x7827FBE24427E27DULL,
	0xFAF9237E1186CF66ULL,
	0x5DD92EE32BBF9ABDULL,
	0x0B68A7AF5F85F7EEULL,
	0x01531713C83FCC39ULL,
	0x76E4F6739A35E8D7ULL,
	0x66033F28DC01923CULL,
	0xFCF34A9B05AE7E6AULL,
	0xE2F6BD41298A2AB9ULL,
	0x14C4257E557B49A1ULL,
	0x1254E65319C6EEFFULL,
	0xC8753773ADF1174CULL,
	0x2170BCAA9FA96E22ULL,
	0x75485627AA225F4DULL,
	0x08717B15BF3C7955ULL,
	0xD19DCF7ACA8D96D6ULL,
	0x9FD609902B4B2E07ULL,
	0xCB26B441FAE4C8CDULL,
	0xA98C7594F55C02F0ULL,
	0x259EE68CD9E76DBAULL,
	0x6A026290FBDB3754ULL,
	0xCF72FD04608D36EDULL,
	0x17F07C2E3A45DB3DULL,
	0xDFAB5841B87802B5ULL,
	0xC050FA06BB0538F6ULL,
	0xAB5CDD3FC321831FULL,
	0xA7B7D1F12395040EULL,
	0x83A2AB72DD8AE992ULL,
	0xBEAF567CC45362F0ULL,
	0x7BB3A77FD8D14783ULL,
	0x8F4098E2470FE0C8ULL,
	0x6AC5C837A2027A6BULL,
	0x302AAD8B1F441D95ULL,
	0x5C909F00088734B9ULL,
	0xF785977C76DE9C77ULL,
	0x1CDAF3931871BEC3ULL,
	0x814E1AB43F3F9345ULL,
	0x1FBE97A317FFBEFAULL,
	0x30581F81528FB27CULL,
	0x4287F49A5BB366DAULL,
	0xD134F430A45C1CF2ULL,
	0x01C82EE0725EDA3AULL,
	0x04C0C50B5BE0CC78ULL,
	0xA26FD104489B3DE5ULL,
	0xEA6C3B8F210A077FULL,
	0x4A738212694AD0B6ULL,
	0x2A430C60DDCC75FFULL,
	0x0A096FB251CFF471ULL,
	0x205AFFCDFBA639CBULL,
	0x32B62CF10571971FULL,
	0xB408D6CDE8E0D4C1ULL,
	0x1DBE03EF5A0059E1ULL,
	0x29D08CEA080FDB84ULL,
	0x3FE91B3FD7F18B37ULL,
	0xF7BECC6682B9EF36ULL,
	0xDCB5C5DC78520BD6ULL,
	0x566DF4A5A9E3341FULL,
	0x9183F8AAA603704DULL,
	0x856D38B447512C51ULL,
	0x1D0614B43A9D6DF9ULL,
	0x00179EB433442A73ULL,
	0x033DBFE564685CF6ULL,
	0x05EC085870DF2AF0ULL,
	0x0626C885AF0EEF65ULL,
	0x070994B8FE881897ULL,
	0x0811E03FCFB84903ULL,
	0x0993C8311127F20EULL,
	0x0A5E25A024FBA6B1ULL,
	0x0C15F03D0FE27E48ULL,
	0x0C2D617E60368120ULL,
	0x0C49BD78283973B0ULL,
	0x0CA696AE84A34281ULL,
	0x0D069101292C9EF9ULL,
	0x0DE2CF914415467AULL,
	0x0DFACE33557D5092ULL,
	0x0FC6144903CFC12BULL,
	0x100C57C7D8B42E58ULL,
	0x1108F594E9F5BCB7ULL,
	0x13DA86340D87FEB9ULL,
	0x141A5383D3AEEB59ULL,
	0x1423888694CDE5FCULL,
	0x153616FF38D5E460ULL,
	0x15D5A13976DCBD05ULL,
	0x15FA2DA0B33C39EEULL,
	0x17ABC9D27E24B4D7ULL,
	0x188E37F5AB501082ULL,
	0x18C32E5601F89AC3ULL,
	0x1A16C07D5F35124FULL,
	0x1A4F317C88C93C04ULL,
	0x1ABF6A7265BBC7AFULL,
	0x1BD281160FB552FDULL,
	0x1BE9A4EEC5B455C5ULL,
	0x1DCF36E171124EFCULL,
	0x1E47BD8DB4D6032FULL,
	0x1FB4F1C56721C87EULL,
	0x225902F0EC8BF0FCULL,
	0x225EADE089BA38D8ULL,
	0x22EE6101A078F310ULL,
	0x23457E4AB5352E38ULL,
	0x236B38CBE43FC318ULL,
	0x264DB70A1A6CC720ULL,
	0x2678A2608A95FAAEULL,
	0x27683B8282673916ULL,
	0x278A558C1E4C9789ULL,
	0x29C4474FA5A650C2ULL,
	0x29E553202112D688ULL,
	0x2A50EBBA4B44FBF5ULL,
	0x2AFA1E90F5548DADULL,
	0x2CDD202075F2BB7BULL,
	0x2D5511A1586B85BDULL,
	0x2D6ADB0D7D20DA8FULL,
	0x323E310D6BD63AC9ULL,
	0x337A168A21969F6CULL,
	0x33801621A7C885FDULL,
	0x34487D8CF061757AULL,
	0x35FC62C0E25BFA2CULL,
	0x3692291361CDD66BULL,
	0x38138436341EB55EULL,
	0x390336A9AC1ADBD3ULL,
	0x41A0F110F49EB86AULL,
	0x41BDEA4884539B22ULL,
	0x41E698459DAD7101ULL,
	0x4223772617A3212AULL,
	0x422ABA4AE3E43AC5ULL,
	0x429E445F8823DE47ULL,
	0x43855C4ABC59CD03ULL,
	0x44324FEB63BAD71BULL,
	0x445C444DDCB144D0ULL,
	0x4570E5C612AD4680ULL,
	0x463B8AC09861DBF2ULL,
	0x46B9652048C5B9EBULL,
	0x477AF884E1779F04ULL,
	0x49E4DB410ACD71E4ULL,
	0x4BA04135745B0C77ULL,
	0x4C3638A33B225E20ULL,
	0x4CA3E38A5F339D85ULL,
	0x4D356B23BBB63ABFULL,
	0x4D81CA32AF4851B8ULL,
	0x4E8FD51F7FDCB494ULL,
	0x4F29CD1DBFF6A704ULL,
	0x50BDE6C6B138D920ULL,
	0x51C45371AA62A30DULL,
	0x53422759826BCE28ULL,
	0x54E0E8C0E9A4CEE7ULL,
	0x550AB6D42C0118D3ULL,
	0x577E78AECCE3D388ULL,
	0x587E8D3EB9594F4AULL,
	0x591C8211EE53BFDBULL,
	0x597B3EA295C88FA7ULL,
	0x59CBA5A49554EB2FULL,
	0x5B1A3C7FC9D58D21ULL,
	0x5B256C6230E168FFULL,
	0x5B657AE4D191AAFBULL,
	0x5B93089CD9316EFEULL,
	0x5C172612CB60DEF2ULL,
	0x5E6EB3F5C47183FFULL,
	0x5E7181FBA91F3766ULL,
	0x5E7B9DCD0092888BULL,
	0x5F1C8C3CFABDC578ULL,
	0x62EDE4DE7880CC68ULL,
	0x637AD198DBD985ECULL,
	0x6444A004E5F352B9ULL,
	0x649F6483822B8C3EULL,
	0x650E0558CFE68DBDULL,
	0x667C9AD6F57610C7ULL,
	0x67AD32C6DFF4B640ULL,
	0x6870068DF23BA71BULL,
	0x69CADB492EBF739EULL,
	0x6B20D9D506B930E6ULL,
	0x6BF0360FC30A1651ULL,
	0x6C3380DB72AFCF88ULL,
	0x6CEAD213E31A6F01ULL,
	0x6D6AD51EAA144766ULL,
	0x6E92CDCE4FEA3B27ULL,
	0x6EA6B9E529B29CC8ULL,
	0x72337416FD82C794ULL,
	0x725E3CA857E0D99FULL,
	0x747A16FD3A3F6970ULL,
	0x75BD0F89A7DF7076ULL,
	0x75E2B6A4145B00DBULL,
	0x77F0A6FE6BE42E62ULL,
	0x7810113EF44E92B2ULL,
	0x78B7C378F1424152ULL,
	0x79383B3295AA93C4ULL,
	0x7AD23E997C77CBEBULL,
	0x7C86AE10DE9A2D24ULL,
	0x7CD6AC9BD4B6C2F1ULL,
	0x7E1F1D367C75D4E3ULL,
	0x7E4C540FC51875ECULL,
	0x7E766271DF1A2F90ULL,
	0x7F7D6EDEF8F5BCFCULL,
	0x801FD0E0D505C316ULL,
	0x8281CFAEB6AE6182ULL,
	0x834E863E91A946E0ULL,
	0x85B1F0C5AFA7684AULL,
	0x85CB9DF9FC580C0EULL,
	0x867D98DC263C7AE1ULL,
	0x86ACAF9277898823ULL,
	0x87413DE10A58AB14ULL,
	0x8831955158E50488ULL,
	0x8A34D884566690F3ULL,
	0x8AA33E951B061C6CULL,
	0x8AB2DCD201956FF5ULL,
	0x8AFF128B2C44402FULL,
	0x929399E254952F89ULL,
	0x93A98F82D5B5FA3DULL,
	0x9440B38F8FC83801ULL,
	0x953B60DFAAD53C53ULL,
	0x96CDB729EF5FF4D8ULL,
	0x96D55B6111ADB046ULL,
	0x994C40C879819D41ULL,
	0x9A4F5BC0D2DF3E7CULL,
	0x9BB64D7C24F7F570ULL,
	0x9CAE05184985E6FAULL,
	0x9DE3C22E44D3D817ULL,
	0x9E10A1C6E2156E96ULL,
	0x9E84167C7BCCAECFULL,
	0xA1651FBBB33533C8ULL,
	0xA1F082630668B975ULL,
	0xA2A3787CEE9A05EEULL,
	0xA6D012ADD54A30CEULL,
	0xA7E2CEFD0280FD3AULL,
	0xA8E1094D80B82D21ULL,
	0xAA15EAD581169812ULL,
	0xAA3EB38B572073F9ULL,
	0xAA7119519C968451ULL,
	0xAAAD5C17B34A935FULL,
	0xABA6C1AFDB427F54ULL,
	0xADC58189215E1C48ULL,
	0xAFEE51897035872EULL,
	0xAFFFA5791F73520AULL,
	0xB032F43502EDFA5AULL,
	0xB23472311441BF43ULL,
	0xB4582D7E642035F5ULL,
	0xB46B23A8B047E71AULL,
	0xB62985BE1B0E820CULL,
	0xB76D3CD9FA80EF23ULL,
	0xB881ACD9C433C39BULL,
	0xB8F711CD1BCCCB5AULL,
	0xBA2586D5321809C9ULL,
	0xBB56A0BAE42B0A58ULL,
	0xBBC9FAFBFB8A476EULL,
	0xBDE676123BBB010AULL,
	0xBE78B15D2D2FFC28ULL,
	0xBF0DA6AAE66E25A0ULL,
	0xBF26C727BFC92EAFULL,
	0xBF874CF500A50632ULL,
	0xBF87DE9EFBA3E7E7ULL,
	0xC132034A5F2F7E10ULL,
	0xC16F60A6BAD360C0ULL,
	0xC2BCD68E7A2AE4AFULL,
	0xC312DAF915B31117ULL,
	0xC63B911BD06869D3ULL,
	0xC65443D5AB354639ULL,
	0xC8167C2D20426AD8ULL,
	0xC88BB3653C3C964FULL,
	0xC926C2CBE0DFF9BEULL,
	0xC9A159E7B047AE82ULL,
	0xC9F63551DC0A7DD3ULL,
	0xC9FDAB91F9DABA18ULL,
	0xCA9C5B912FFBCAC7ULL,
	0xCB4D2A3F81E1794EULL,
	0xCBBACF102B89411FULL,
	0xD0C5486A7AD05CF9ULL,
	0xD362C01C25B71B50ULL,
	0xD3CD986A5533AE96ULL,
	0xD628DFD6FD4C4629ULL,
	0xD843A7FFCE96C7AEULL,
	0xD8EDA6A4C2799AFCULL,
	0xDB37969AE172E0A2ULL,
	0xDBD4F370AE374627ULL,
	0xDC692CF534CF6094ULL,
	0xDE7D8C810DEBCCBBULL,
	0xDFDBE74EFD0C1D9AULL,
	0xE186FB75B0C94A7CULL,
	0xE26933D3B03457B8ULL,
	0xE2D4CD545D19C5B3ULL,
	0xE321D1E0F5FD72D1ULL,
	0xE50F70F06A579FCCULL,
	0xE6DE529515D5322EULL,
	0xE6F58B08B45D7A12ULL,
	0xE7357F7B88B8338DULL,
	0xE8F494F7E371D157ULL,
	0xE995E22AF7111D57ULL,
	0xEA9589F00A338035ULL,
	0xEAC344DACB040496ULL,
	0xEB5B9AA992D0582BULL,
	0xEB824892D843D365ULL,
	0xEC434CD731FF1D6EULL,
	0xED4548400C6CD9B3ULL,
	0xED85FA32ABE4D31AULL,
	0xEDB14BC9682C3EC7ULL,
	0xEFD8CF7039C9E3ABULL,
	0xEFF36C5D384458ADULL,
	0xF04AF4C9E8BD1063ULL,
	0xF0C135866D740895ULL,
	0xF399E2091BB4EB3DULL,
	0xF4941ADAEFD62B50ULL,
	0xF4A0D7C6BE16E9E6ULL,
	0xF5BC79A1093D4297ULL,
	0xF88CD79C16ED40C1ULL,
	0xF9B971BF3BAE0F5FULL,
	0xFA6C85CFB99D0738ULL,
	0xFB90D71D100E9595ULL,
	0xFD06442092131C5EULL,
	0xFD0BA919048A69AAULL,
	0xFF08D3B90FB93B8CULL,
	0xFF7C9A1B789D0D42ULL
};

bool Parser::TryParse(std::shared_ptr<Bytebuffer>& buffer, Layout& out)
{
	Layout::Header& header = out.header;
	if (!buffer->Get(header))
		return false;

	u32 numSections = header.sectionCount;
	if (!LoadArrayOfStructs(buffer, numSections * sizeof(Layout::SectionHeader), out.sectionHeaders))
		return false;

	u32 numFields = header.totalFieldCount;
	if (!LoadArrayOfStructs(buffer, numFields * sizeof(Layout::FieldStructure), out.fieldStructures))
		return false;

	if (!LoadArrayOfStructs(buffer, header.fieldStorageInfoSize, out.fieldStorageInfos))
		return false;

	if (header.palletDataSize)
	{
		out.perFieldPalleteData.resize(numFields);

		//size_t palletStartBufferIndex = buffer->readData;
		for (u32 i = 0; i < numFields; i++)
		{
			const Layout::FieldStorageInfo& fieldInfo = out.fieldStorageInfos[i];

			if (fieldInfo.compressionType != Layout::FieldStorageInfo::CompressionType::BitpackedIndexed &&
				fieldInfo.compressionType != Layout::FieldStorageInfo::CompressionType::BitpackedIndexedArray)
				continue;

			std::vector<u32>& fieldPaletteData = out.perFieldPalleteData[i];

			u32 extraFieldDataInIntegers = fieldInfo.extraDataSize / 4;
			fieldPaletteData.resize(extraFieldDataInIntegers);

			if (!buffer->GetBytes(reinterpret_cast<u8*>(&fieldPaletteData[0]), fieldInfo.extraDataSize))
				return false;
		}

		//size_t palletReadData = buffer->readData - palletStartBufferIndex;
		//assert(palletReadData == header.palletDataSize);
	}

	if (header.commonDataSize)
	{
		out.perFieldCommonDataIDToValue.resize(numFields);

		//size_t commonDataStartBufferIndex = buffer->readData;
		for (u32 i = 0; i < numFields; i++)
		{
			const Layout::FieldStorageInfo& fieldInfo = out.fieldStorageInfos[i];

			if (fieldInfo.compressionType != Layout::FieldStorageInfo::CompressionType::CommonData)
				continue;

			robin_hood::unordered_map<u32, u32>& fieldCommonData = out.perFieldCommonDataIDToValue[i];

			u32 extraFieldDataInIntegerPairs = fieldInfo.extraDataSize / 8;
			for (u32 j = 0; j < extraFieldDataInIntegerPairs; j++)
			{
				u32 id = 0;
				u32 value = 0;

				if (!buffer->GetU32(id))
					return false;

				if (!buffer->GetU32(value))
					return false;

				fieldCommonData[id] = value;
			}
		}

		//size_t commonDataReadData = buffer->readData - commonDataStartBufferIndex;
		//assert(commonDataReadData == header.commonDataSize);
	}

	out.sections.resize(numSections);
	for (u32 i = 0; i < numSections; i++)
	{
		Layout::SectionHeader& sectionHeader = out.sectionHeaders[i];
		Layout::Section& section = out.sections[i];

		// Ensure buffer is aligned with the expected buffer offset for this section
		assert(sectionHeader.fileOffset == buffer->readData);

		bool isEncrypted = sectionHeader.tactKeyHash != 0 && _cascEncryptionKeyLookup.find(sectionHeader.tactKeyHash) == _cascEncryptionKeyLookup.end();
		section.isEncrypted = isEncrypted;

		if (header.flags.HasOffsetMap)
		{
			u32 variableRecordDataSize = sectionHeader.recordEndOffset - sectionHeader.fileOffset;
			section.variableRecordData = buffer->GetReadPointer();

			buffer->SkipRead(variableRecordDataSize);

			// Ensure buffer is aligned with where we expect recordEnd to be.
			assert(sectionHeader.recordEndOffset == buffer->readData);
		}
		else
		{
			u32 totalRecordSize = sectionHeader.recordCount * header.recordSize;
			section.recordData = buffer->GetReadPointer();
			buffer->SkipRead(totalRecordSize);

			section.stringTableData = buffer->GetReadPointer();
			buffer->SkipRead(sectionHeader.stringTableSize);
		}

		if (sectionHeader.idListSize)
		{
			section.idListData = reinterpret_cast<u32*>(buffer->GetReadPointer());
			buffer->SkipRead(sectionHeader.idListSize);
		}

		if (sectionHeader.copyTableCount)
		{
			section.copyTable = reinterpret_cast<Layout::CopyTableEntry*>(buffer->GetReadPointer());

			u32 copyTableSizeInBytes = sectionHeader.copyTableCount * sizeof(Layout::CopyTableEntry);
			buffer->SkipRead(copyTableSizeInBytes);
		}

		/*
			There supposedly is issues reading "unittestsparse.db2" without doing the following
			(The if checks the table_hash against unittestsparse's table hash)

			if (header->table_hash == 145293629)
				currentOffset+=itemSectionHeader.offset_map_id_count*4;
		*/

		// Read Offset Map List
		if (sectionHeader.offsetMapIDCount)
		{
			section.offsetMapData = reinterpret_cast<Layout::OffsetMapEntry*>(buffer->GetReadPointer());

			u32 offsetMapDataSizeInBytes = sectionHeader.offsetMapIDCount * sizeof(Layout::OffsetMapEntry);
			buffer->SkipRead(offsetMapDataSizeInBytes);
		}

		if (sectionHeader.relationshipMapDataSize)
		{
			//size_t relationshipMapStartBufferIndex = buffer->readData;

			if (section.isEncrypted)
			{
				buffer->SkipRead(sectionHeader.relationshipMapDataSize);
			}
			else
			{
				if (!buffer->GetU32(section.relationshipMap.entriesCount))
					return false;

				if (!buffer->GetU32(section.relationshipMap.minID))
					return false;

				if (!buffer->GetU32(section.relationshipMap.maxID))
					return false;

				section.relationshipMap.entries = reinterpret_cast<Layout::RelationshipMapEntry*>(buffer->GetReadPointer());

				u32 relationshipMapEntriesSizeInBytes = section.relationshipMap.entriesCount * sizeof(Layout::RelationshipMapEntry);
				buffer->SkipRead(relationshipMapEntriesSizeInBytes);

				// Setup HashMap for this Section
				{
					for (u32 j = 0; j < section.relationshipMap.entriesCount; j++)
					{
						const Layout::RelationshipMapEntry& relationshipMapEntry = section.relationshipMap.entries[j];
						section.recordIndexToForeignID[relationshipMapEntry.recordIndex] = relationshipMapEntry.foreignID;
					}
				}
			}

			//size_t relationshipMapReadData = buffer->readData - relationshipMapStartBufferIndex;
			//assert(relationshipMapReadData == sectionHeader.relationshipMapDataSize);
		}

		// Read Offset Map ID List
		if (sectionHeader.offsetMapIDCount)
		{
			section.offsetMapList = reinterpret_cast<u32*>(buffer->GetReadPointer());

			u32 offsetMapListSizeInBytes = sectionHeader.offsetMapIDCount * sizeof(u32);
			buffer->SkipRead(offsetMapListSizeInBytes);
		}
	}

	return true;
}
bool Parser::TryReadRecord(const Layout& layout, u32 index, u32& outSectionID, u32& outRecordID, u8*& outRecordData)
{
	PRAGMA_CLANG_DIAGNOSTIC_PUSH;
	PRAGMA_CLANG_DIAGNOSTIC_IGNORE(-Wunused-value);
	const Layout::Header& header = layout.header;

	u32 sectionIndex = 0;
	u32 localIndex = index;

	for (sectionIndex; sectionIndex < header.sectionCount; sectionIndex++)
	{
		const Layout::SectionHeader& sectionHeader = layout.sectionHeaders[sectionIndex];
		if (localIndex < sectionHeader.recordCount)
			break;

		localIndex -= sectionHeader.recordCount;
	}

	const Layout::SectionHeader& sectionHeader = layout.sectionHeaders[sectionIndex];
	const Layout::Section& section = layout.sections[sectionIndex];

	if (section.isEncrypted)
		return false;

	u32 recordID = -1;
	u8* recordData = nullptr;

	// Calculate RecordID & RecordData
	{
		if (header.flags.HasOffsetMap)
		{
			recordID = section.offsetMapList[localIndex];

			const Layout::OffsetMapEntry& offsetMapEntry = section.offsetMapData[localIndex];
			recordData = (section.variableRecordData + offsetMapEntry.offset) - sectionHeader.fileOffset;
		}
		else
		{
			if (sectionHeader.idListSize)
				recordID = section.idListData[localIndex];

			if (!recordID)
				recordID = header.minID + index;

			recordData = &section.recordData[localIndex * header.recordSize];
		}
	}

	outSectionID = sectionIndex;
	outRecordID = recordID;
	outRecordData = recordData;

	return true;
	PRAGMA_CLANG_DIAGNOSTIC_POP;
}

char* Parser::GetString(const Layout& layout, u32 recordIndex, u32 fieldIndex)
{
	return GetStringInArr(layout, recordIndex, fieldIndex, 0);
}
char* Parser::GetStringInArr(const Layout& layout, u32 recordIndex, u32 fieldIndex, u32 arrIndex)
{
	PRAGMA_CLANG_DIAGNOSTIC_PUSH;
	PRAGMA_CLANG_DIAGNOSTIC_IGNORE(-Wunused-value);
	const Layout::Header& header = layout.header;

	u32 sectionIndex = 0;
	u32 localIndex = recordIndex;

	for (sectionIndex; sectionIndex < header.sectionCount; sectionIndex++)
	{
		const Layout::SectionHeader& sectionHeader = layout.sectionHeaders[sectionIndex];
		if (localIndex < sectionHeader.recordCount)
			break;

		localIndex -= sectionHeader.recordCount;
	}

	// Validate sectionIndex to make sure we're not accessing a non existing section
	u32 numSections = static_cast<u32>(layout.sections.size());
	if (sectionIndex >= numSections)
		return new char[1]{'\0'};

	// Validate fieldIndex to make sure we're not accessing a non existing field
	if (fieldIndex >= layout.fieldStorageInfos.size() ||
		fieldIndex >= layout.fieldStructures.size())
		return new char[1]{'\0'};

	const Layout::Section& section = layout.sections[sectionIndex];
	const Layout::FieldStorageInfo& fieldStorageInfo = layout.fieldStorageInfos[fieldIndex];

	// Get Field Value
	u8* recordData = &section.recordData[localIndex * header.recordSize];
	u32 fieldOffset = layout.fieldStructures[fieldIndex].offset;
	u32 fieldOffsetInBytes = fieldStorageInfo.offsetInBits >> 3;

	u32 stringOffset = reinterpret_cast<u32*>(recordData + fieldOffsetInBytes)[arrIndex];

	if (stringOffset != 0)
	{
		stringOffset += arrIndex * fieldOffset;
		stringOffset += fieldOffsetInBytes;
		stringOffset += recordIndex * header.recordSize;
		stringOffset -= header.recordCount * header.recordSize;
	}

	u32 strSectionIndex = 0;
	for (strSectionIndex; strSectionIndex < numSections; strSectionIndex++)
	{
		const Layout::SectionHeader& sectionHeader = layout.sectionHeaders[strSectionIndex];
		if (stringOffset < sectionHeader.stringTableSize)
			break;

		stringOffset -= sectionHeader.stringTableSize;
	}

	const Layout::Section& strSection = layout.sections[strSectionIndex];
	char* string = reinterpret_cast<char*>(&strSection.stringTableData[stringOffset]);

	return string;
	PRAGMA_CLANG_DIAGNOSTIC_POP;
}
u32 Parser::GetFieldSize(const Layout& layout, u32 fieldIndex)
{
	const Layout::Header& header = layout.header;

	if (fieldIndex >= header.totalFieldCount)
		return 0;

	const Layout::FieldStorageInfo& fieldStorageInfo = layout.fieldStorageInfos[fieldIndex];

	switch (fieldStorageInfo.compressionType)
	{
	case Layout::FieldStorageInfo::CompressionType::None:
	case Layout::FieldStorageInfo::CompressionType::CommonData:
	case Layout::FieldStorageInfo::CompressionType::Bitpacked:
	case Layout::FieldStorageInfo::CompressionType::BitpackedSigned:
		return RoundBitToNextByte(fieldStorageInfo.sizeInBits);

	case Layout::FieldStorageInfo::CompressionType::BitpackedIndexed:
		return sizeof(u32);

	case Layout::FieldStorageInfo::CompressionType::BitpackedIndexedArray:
		return fieldStorageInfo.fieldBitpackedIndexedArray.arrayCount * sizeof(u32);
	}
	return RoundBitToNextByte(fieldStorageInfo.sizeInBits);
}

bool Parser::IsRecordEncrypted(const Layout& layout, u32 recordIndex)
{
	PRAGMA_CLANG_DIAGNOSTIC_PUSH;
	PRAGMA_CLANG_DIAGNOSTIC_IGNORE(-Wunused-value);
	const Layout::Header& header = layout.header;

	u32 sectionIndex = 0;
	u32 localIndex = recordIndex;

	for (sectionIndex; sectionIndex < header.sectionCount; sectionIndex++)
	{
		const Layout::SectionHeader& sectionHeader = layout.sectionHeaders[sectionIndex];
		if (localIndex < sectionHeader.recordCount)
			break;

		localIndex -= sectionHeader.recordCount;
	}

	if (sectionIndex >= layout.sections.size())
		return true;

	const Layout::Section& section = layout.sections[sectionIndex];
	return section.isEncrypted;
	PRAGMA_CLANG_DIAGNOSTIC_POP;
}
u32 Parser::GetRecordIDFromIndex(const Layout& layout, u32 recordIndex)
{
	PRAGMA_CLANG_DIAGNOSTIC_PUSH;
	PRAGMA_CLANG_DIAGNOSTIC_IGNORE(-Wunused-value);
	const Layout::Header& header = layout.header;

	u32 recordID = -1;
	u32 sectionIndex = 0;
	u32 localIndex = recordIndex;

	for (sectionIndex; sectionIndex < header.sectionCount; sectionIndex++)
	{
		const Layout::SectionHeader& sectionHeader = layout.sectionHeaders[sectionIndex];
		if (localIndex < sectionHeader.recordCount)
			break;

		localIndex -= sectionHeader.recordCount;
	}

	const Layout::SectionHeader& sectionHeader = layout.sectionHeaders[sectionIndex];
	const Layout::Section& section = layout.sections[sectionIndex];

	if (section.isEncrypted)
		return recordID;

	if (header.flags.HasOffsetMap)
	{
		recordID = section.offsetMapList[localIndex];
	}
	else
	{
		if (sectionHeader.idListSize)
			recordID = section.idListData[localIndex];

		if (!recordID)
			recordID = header.minID + recordID;
	}

	return recordID;
	PRAGMA_CLANG_DIAGNOSTIC_POP;
}

u32 Parser::RoundBitToNextByte(u32 bits)
{
	u32 value = ((bits + 7) & (-8));
	value += 8 * (value == 24);

	u32 bytes = value >> 3;
	return bytes;
}
bool Parser::GetBitValue(const u8* fieldData, u32 bitIndex)
{
	u32 byteOffset = bitIndex >> 0x3;

	i32 byteValue = static_cast<i32>(fieldData[byteOffset]);
	i32 bitmask = 1 << (bitIndex & 0x7);

	return ((byteValue & bitmask) != 0);
}
u32 Parser::GetU32FromBits(const u8* fieldData, u32 bitOffset, u32 bitsToRead)
{
	u32 result = 0;

	for (u32 i = 0; i < bitsToRead; i++)
	{
		result |= (GetBitValue(fieldData, bitOffset + i) << i);
	}

	return result;
}
