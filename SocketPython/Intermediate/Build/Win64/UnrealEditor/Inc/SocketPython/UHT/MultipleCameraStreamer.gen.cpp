// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SocketPython/Public/MultipleCameraStreamer.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeMultipleCameraStreamer() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_AActor();
SOCKETPYTHON_API UClass* Z_Construct_UClass_AMultipleCameraStreamer();
SOCKETPYTHON_API UClass* Z_Construct_UClass_AMultipleCameraStreamer_NoRegister();
SOCKETPYTHON_API UScriptStruct* Z_Construct_UScriptStruct_FRenderRequest2();
UPackage* Z_Construct_UPackage__Script_SocketPython();
// End Cross Module References

// Begin ScriptStruct FRenderRequest2
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_RenderRequest2;
class UScriptStruct* FRenderRequest2::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_RenderRequest2.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_RenderRequest2.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FRenderRequest2, (UObject*)Z_Construct_UPackage__Script_SocketPython(), TEXT("RenderRequest2"));
	}
	return Z_Registration_Info_UScriptStruct_RenderRequest2.OuterSingleton;
}
template<> SOCKETPYTHON_API UScriptStruct* StaticStruct<FRenderRequest2>()
{
	return FRenderRequest2::StaticStruct();
}
struct Z_Construct_UScriptStruct_FRenderRequest2_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/MultipleCameraStreamer.h" },
	};
#endif // WITH_METADATA
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FRenderRequest2>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
};
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FRenderRequest2_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_SocketPython,
	nullptr,
	&NewStructOps,
	"RenderRequest2",
	nullptr,
	0,
	sizeof(FRenderRequest2),
	alignof(FRenderRequest2),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FRenderRequest2_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FRenderRequest2_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FRenderRequest2()
{
	if (!Z_Registration_Info_UScriptStruct_RenderRequest2.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_RenderRequest2.InnerSingleton, Z_Construct_UScriptStruct_FRenderRequest2_Statics::StructParams);
	}
	return Z_Registration_Info_UScriptStruct_RenderRequest2.InnerSingleton;
}
// End ScriptStruct FRenderRequest2

// Begin Class AMultipleCameraStreamer
void AMultipleCameraStreamer::StaticRegisterNativesAMultipleCameraStreamer()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(AMultipleCameraStreamer);
UClass* Z_Construct_UClass_AMultipleCameraStreamer_NoRegister()
{
	return AMultipleCameraStreamer::StaticClass();
}
struct Z_Construct_UClass_AMultipleCameraStreamer_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "MultipleCameraStreamer.h" },
		{ "ModuleRelativePath", "Public/MultipleCameraStreamer.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AMultipleCameraStreamer>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_AMultipleCameraStreamer_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AActor,
	(UObject* (*)())Z_Construct_UPackage__Script_SocketPython,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AMultipleCameraStreamer_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_AMultipleCameraStreamer_Statics::ClassParams = {
	&AMultipleCameraStreamer::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_AMultipleCameraStreamer_Statics::Class_MetaDataParams), Z_Construct_UClass_AMultipleCameraStreamer_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_AMultipleCameraStreamer()
{
	if (!Z_Registration_Info_UClass_AMultipleCameraStreamer.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AMultipleCameraStreamer.OuterSingleton, Z_Construct_UClass_AMultipleCameraStreamer_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_AMultipleCameraStreamer.OuterSingleton;
}
template<> SOCKETPYTHON_API UClass* StaticClass<AMultipleCameraStreamer>()
{
	return AMultipleCameraStreamer::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(AMultipleCameraStreamer);
AMultipleCameraStreamer::~AMultipleCameraStreamer() {}
// End Class AMultipleCameraStreamer

// Begin Registration
struct Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_MultipleCameraStreamer_h_Statics
{
	static constexpr FStructRegisterCompiledInInfo ScriptStructInfo[] = {
		{ FRenderRequest2::StaticStruct, Z_Construct_UScriptStruct_FRenderRequest2_Statics::NewStructOps, TEXT("RenderRequest2"), &Z_Registration_Info_UScriptStruct_RenderRequest2, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FRenderRequest2), 1498929233U) },
	};
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_AMultipleCameraStreamer, AMultipleCameraStreamer::StaticClass, TEXT("AMultipleCameraStreamer"), &Z_Registration_Info_UClass_AMultipleCameraStreamer, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AMultipleCameraStreamer), 2242332078U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_MultipleCameraStreamer_h_591443545(TEXT("/Script/SocketPython"),
	Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_MultipleCameraStreamer_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_MultipleCameraStreamer_h_Statics::ClassInfo),
	Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_MultipleCameraStreamer_h_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_MultipleCameraStreamer_h_Statics::ScriptStructInfo),
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
