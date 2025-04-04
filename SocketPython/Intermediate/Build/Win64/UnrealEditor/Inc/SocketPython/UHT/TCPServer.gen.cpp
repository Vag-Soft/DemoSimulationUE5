// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SocketPython/Public/TCPServer.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeTCPServer() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_AActor();
SOCKETPYTHON_API UClass* Z_Construct_UClass_ATCPServer();
SOCKETPYTHON_API UClass* Z_Construct_UClass_ATCPServer_NoRegister();
SOCKETPYTHON_API UScriptStruct* Z_Construct_UScriptStruct_FRenderRequestStruct();
UPackage* Z_Construct_UPackage__Script_SocketPython();
// End Cross Module References

// Begin ScriptStruct FRenderRequestStruct
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_RenderRequestStruct;
class UScriptStruct* FRenderRequestStruct::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_RenderRequestStruct.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_RenderRequestStruct.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FRenderRequestStruct, (UObject*)Z_Construct_UPackage__Script_SocketPython(), TEXT("RenderRequestStruct"));
	}
	return Z_Registration_Info_UScriptStruct_RenderRequestStruct.OuterSingleton;
}
template<> SOCKETPYTHON_API UScriptStruct* StaticStruct<FRenderRequestStruct>()
{
	return FRenderRequestStruct::StaticStruct();
}
struct Z_Construct_UScriptStruct_FRenderRequestStruct_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/TCPServer.h" },
	};
#endif // WITH_METADATA
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FRenderRequestStruct>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
};
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FRenderRequestStruct_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_SocketPython,
	nullptr,
	&NewStructOps,
	"RenderRequestStruct",
	nullptr,
	0,
	sizeof(FRenderRequestStruct),
	alignof(FRenderRequestStruct),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FRenderRequestStruct_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FRenderRequestStruct_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FRenderRequestStruct()
{
	if (!Z_Registration_Info_UScriptStruct_RenderRequestStruct.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_RenderRequestStruct.InnerSingleton, Z_Construct_UScriptStruct_FRenderRequestStruct_Statics::StructParams);
	}
	return Z_Registration_Info_UScriptStruct_RenderRequestStruct.InnerSingleton;
}
// End ScriptStruct FRenderRequestStruct

// Begin Class ATCPServer
void ATCPServer::StaticRegisterNativesATCPServer()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(ATCPServer);
UClass* Z_Construct_UClass_ATCPServer_NoRegister()
{
	return ATCPServer::StaticClass();
}
struct Z_Construct_UClass_ATCPServer_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "TCPServer.h" },
		{ "ModuleRelativePath", "Public/TCPServer.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ATCPServer>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_ATCPServer_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AActor,
	(UObject* (*)())Z_Construct_UPackage__Script_SocketPython,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ATCPServer_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ATCPServer_Statics::ClassParams = {
	&ATCPServer::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ATCPServer_Statics::Class_MetaDataParams), Z_Construct_UClass_ATCPServer_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_ATCPServer()
{
	if (!Z_Registration_Info_UClass_ATCPServer.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ATCPServer.OuterSingleton, Z_Construct_UClass_ATCPServer_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ATCPServer.OuterSingleton;
}
template<> SOCKETPYTHON_API UClass* StaticClass<ATCPServer>()
{
	return ATCPServer::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(ATCPServer);
ATCPServer::~ATCPServer() {}
// End Class ATCPServer

// Begin Registration
struct Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_DemoSimulationUE5_SocketPython_Source_SocketPython_Public_TCPServer_h_Statics
{
	static constexpr FStructRegisterCompiledInInfo ScriptStructInfo[] = {
		{ FRenderRequestStruct::StaticStruct, Z_Construct_UScriptStruct_FRenderRequestStruct_Statics::NewStructOps, TEXT("RenderRequestStruct"), &Z_Registration_Info_UScriptStruct_RenderRequestStruct, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FRenderRequestStruct), 1245797344U) },
	};
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ATCPServer, ATCPServer::StaticClass, TEXT("ATCPServer"), &Z_Registration_Info_UClass_ATCPServer, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ATCPServer), 2321336042U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_DemoSimulationUE5_SocketPython_Source_SocketPython_Public_TCPServer_h_2040553201(TEXT("/Script/SocketPython"),
	Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_DemoSimulationUE5_SocketPython_Source_SocketPython_Public_TCPServer_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_DemoSimulationUE5_SocketPython_Source_SocketPython_Public_TCPServer_h_Statics::ClassInfo),
	Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_DemoSimulationUE5_SocketPython_Source_SocketPython_Public_TCPServer_h_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_DemoSimulationUE5_SocketPython_Source_SocketPython_Public_TCPServer_h_Statics::ScriptStructInfo),
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
