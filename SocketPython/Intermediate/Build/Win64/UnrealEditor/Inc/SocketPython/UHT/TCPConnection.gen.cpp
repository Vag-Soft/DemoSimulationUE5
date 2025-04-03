// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SocketPython/Public/TCPConnection.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeTCPConnection() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_AActor();
SOCKETPYTHON_API UClass* Z_Construct_UClass_ATCPConnection();
SOCKETPYTHON_API UClass* Z_Construct_UClass_ATCPConnection_NoRegister();
UPackage* Z_Construct_UPackage__Script_SocketPython();
// End Cross Module References

// Begin Class ATCPConnection
void ATCPConnection::StaticRegisterNativesATCPConnection()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(ATCPConnection);
UClass* Z_Construct_UClass_ATCPConnection_NoRegister()
{
	return ATCPConnection::StaticClass();
}
struct Z_Construct_UClass_ATCPConnection_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "TCPConnection.h" },
		{ "ModuleRelativePath", "Public/TCPConnection.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ATCPConnection>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_ATCPConnection_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AActor,
	(UObject* (*)())Z_Construct_UPackage__Script_SocketPython,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ATCPConnection_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ATCPConnection_Statics::ClassParams = {
	&ATCPConnection::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x009000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ATCPConnection_Statics::Class_MetaDataParams), Z_Construct_UClass_ATCPConnection_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_ATCPConnection()
{
	if (!Z_Registration_Info_UClass_ATCPConnection.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ATCPConnection.OuterSingleton, Z_Construct_UClass_ATCPConnection_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ATCPConnection.OuterSingleton;
}
template<> SOCKETPYTHON_API UClass* StaticClass<ATCPConnection>()
{
	return ATCPConnection::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(ATCPConnection);
ATCPConnection::~ATCPConnection() {}
// End Class ATCPConnection

// Begin Registration
struct Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_TCPConnection_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ATCPConnection, ATCPConnection::StaticClass, TEXT("ATCPConnection"), &Z_Registration_Info_UClass_ATCPConnection, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ATCPConnection), 2556014680U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_TCPConnection_h_3235902458(TEXT("/Script/SocketPython"),
	Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_TCPConnection_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_TCPConnection_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
