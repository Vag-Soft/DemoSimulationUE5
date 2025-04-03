// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SocketPython/Public/OneCameraStreamer.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeOneCameraStreamer() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_AActor();
SOCKETPYTHON_API UClass* Z_Construct_UClass_AOneCameraStreamer();
SOCKETPYTHON_API UClass* Z_Construct_UClass_AOneCameraStreamer_NoRegister();
SOCKETPYTHON_API UScriptStruct* Z_Construct_UScriptStruct_FRenderRequest();
UPackage* Z_Construct_UPackage__Script_SocketPython();
// End Cross Module References

// Begin ScriptStruct FRenderRequest
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_RenderRequest;
class UScriptStruct* FRenderRequest::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_RenderRequest.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_RenderRequest.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FRenderRequest, (UObject*)Z_Construct_UPackage__Script_SocketPython(), TEXT("RenderRequest"));
	}
	return Z_Registration_Info_UScriptStruct_RenderRequest.OuterSingleton;
}
template<> SOCKETPYTHON_API UScriptStruct* StaticStruct<FRenderRequest>()
{
	return FRenderRequest::StaticStruct();
}
struct Z_Construct_UScriptStruct_FRenderRequest_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/OneCameraStreamer.h" },
	};
#endif // WITH_METADATA
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FRenderRequest>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
};
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FRenderRequest_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_SocketPython,
	nullptr,
	&NewStructOps,
	"RenderRequest",
	nullptr,
	0,
	sizeof(FRenderRequest),
	alignof(FRenderRequest),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FRenderRequest_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FRenderRequest_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FRenderRequest()
{
	if (!Z_Registration_Info_UScriptStruct_RenderRequest.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_RenderRequest.InnerSingleton, Z_Construct_UScriptStruct_FRenderRequest_Statics::StructParams);
	}
	return Z_Registration_Info_UScriptStruct_RenderRequest.InnerSingleton;
}
// End ScriptStruct FRenderRequest

// Begin Class AOneCameraStreamer
void AOneCameraStreamer::StaticRegisterNativesAOneCameraStreamer()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(AOneCameraStreamer);
UClass* Z_Construct_UClass_AOneCameraStreamer_NoRegister()
{
	return AOneCameraStreamer::StaticClass();
}
struct Z_Construct_UClass_AOneCameraStreamer_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "OneCameraStreamer.h" },
		{ "ModuleRelativePath", "Public/OneCameraStreamer.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AOneCameraStreamer>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_AOneCameraStreamer_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AActor,
	(UObject* (*)())Z_Construct_UPackage__Script_SocketPython,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AOneCameraStreamer_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_AOneCameraStreamer_Statics::ClassParams = {
	&AOneCameraStreamer::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_AOneCameraStreamer_Statics::Class_MetaDataParams), Z_Construct_UClass_AOneCameraStreamer_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_AOneCameraStreamer()
{
	if (!Z_Registration_Info_UClass_AOneCameraStreamer.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AOneCameraStreamer.OuterSingleton, Z_Construct_UClass_AOneCameraStreamer_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_AOneCameraStreamer.OuterSingleton;
}
template<> SOCKETPYTHON_API UClass* StaticClass<AOneCameraStreamer>()
{
	return AOneCameraStreamer::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(AOneCameraStreamer);
AOneCameraStreamer::~AOneCameraStreamer() {}
// End Class AOneCameraStreamer

// Begin Registration
struct Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_OneCameraStreamer_h_Statics
{
	static constexpr FStructRegisterCompiledInInfo ScriptStructInfo[] = {
		{ FRenderRequest::StaticStruct, Z_Construct_UScriptStruct_FRenderRequest_Statics::NewStructOps, TEXT("RenderRequest"), &Z_Registration_Info_UScriptStruct_RenderRequest, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FRenderRequest), 820134313U) },
	};
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_AOneCameraStreamer, AOneCameraStreamer::StaticClass, TEXT("AOneCameraStreamer"), &Z_Registration_Info_UClass_AOneCameraStreamer, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AOneCameraStreamer), 2160524607U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_OneCameraStreamer_h_2284751931(TEXT("/Script/SocketPython"),
	Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_OneCameraStreamer_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_OneCameraStreamer_h_Statics::ClassInfo),
	Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_OneCameraStreamer_h_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_OneCameraStreamer_h_Statics::ScriptStructInfo),
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
