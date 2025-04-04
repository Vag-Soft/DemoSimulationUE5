// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SocketPython/Public/BackBuffer.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeBackBuffer() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_AActor();
SOCKETPYTHON_API UClass* Z_Construct_UClass_ABackBuffer();
SOCKETPYTHON_API UClass* Z_Construct_UClass_ABackBuffer_NoRegister();
SOCKETPYTHON_API UScriptStruct* Z_Construct_UScriptStruct_FRenderRequest1();
UPackage* Z_Construct_UPackage__Script_SocketPython();
// End Cross Module References

// Begin ScriptStruct FRenderRequest1
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_RenderRequest1;
class UScriptStruct* FRenderRequest1::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_RenderRequest1.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_RenderRequest1.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FRenderRequest1, (UObject*)Z_Construct_UPackage__Script_SocketPython(), TEXT("RenderRequest1"));
	}
	return Z_Registration_Info_UScriptStruct_RenderRequest1.OuterSingleton;
}
template<> SOCKETPYTHON_API UScriptStruct* StaticStruct<FRenderRequest1>()
{
	return FRenderRequest1::StaticStruct();
}
struct Z_Construct_UScriptStruct_FRenderRequest1_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/BackBuffer.h" },
	};
#endif // WITH_METADATA
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FRenderRequest1>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
};
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FRenderRequest1_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_SocketPython,
	nullptr,
	&NewStructOps,
	"RenderRequest1",
	nullptr,
	0,
	sizeof(FRenderRequest1),
	alignof(FRenderRequest1),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FRenderRequest1_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FRenderRequest1_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FRenderRequest1()
{
	if (!Z_Registration_Info_UScriptStruct_RenderRequest1.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_RenderRequest1.InnerSingleton, Z_Construct_UScriptStruct_FRenderRequest1_Statics::StructParams);
	}
	return Z_Registration_Info_UScriptStruct_RenderRequest1.InnerSingleton;
}
// End ScriptStruct FRenderRequest1

// Begin Class ABackBuffer
void ABackBuffer::StaticRegisterNativesABackBuffer()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(ABackBuffer);
UClass* Z_Construct_UClass_ABackBuffer_NoRegister()
{
	return ABackBuffer::StaticClass();
}
struct Z_Construct_UClass_ABackBuffer_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "BackBuffer.h" },
		{ "ModuleRelativePath", "Public/BackBuffer.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ABackBuffer>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_ABackBuffer_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AActor,
	(UObject* (*)())Z_Construct_UPackage__Script_SocketPython,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ABackBuffer_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ABackBuffer_Statics::ClassParams = {
	&ABackBuffer::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ABackBuffer_Statics::Class_MetaDataParams), Z_Construct_UClass_ABackBuffer_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_ABackBuffer()
{
	if (!Z_Registration_Info_UClass_ABackBuffer.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ABackBuffer.OuterSingleton, Z_Construct_UClass_ABackBuffer_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ABackBuffer.OuterSingleton;
}
template<> SOCKETPYTHON_API UClass* StaticClass<ABackBuffer>()
{
	return ABackBuffer::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(ABackBuffer);
ABackBuffer::~ABackBuffer() {}
// End Class ABackBuffer

// Begin Registration
struct Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_DemoSimulationUE5_SocketPython_Source_SocketPython_Public_BackBuffer_h_Statics
{
	static constexpr FStructRegisterCompiledInInfo ScriptStructInfo[] = {
		{ FRenderRequest1::StaticStruct, Z_Construct_UScriptStruct_FRenderRequest1_Statics::NewStructOps, TEXT("RenderRequest1"), &Z_Registration_Info_UScriptStruct_RenderRequest1, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FRenderRequest1), 1907502061U) },
	};
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ABackBuffer, ABackBuffer::StaticClass, TEXT("ABackBuffer"), &Z_Registration_Info_UClass_ABackBuffer, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ABackBuffer), 1231697042U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_DemoSimulationUE5_SocketPython_Source_SocketPython_Public_BackBuffer_h_3120223545(TEXT("/Script/SocketPython"),
	Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_DemoSimulationUE5_SocketPython_Source_SocketPython_Public_BackBuffer_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_DemoSimulationUE5_SocketPython_Source_SocketPython_Public_BackBuffer_h_Statics::ClassInfo),
	Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_DemoSimulationUE5_SocketPython_Source_SocketPython_Public_BackBuffer_h_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_DemoSimulationUE5_SocketPython_Source_SocketPython_Public_BackBuffer_h_Statics::ScriptStructInfo),
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
