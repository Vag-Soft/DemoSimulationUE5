// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SocketPython/SocketPythonGameMode.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeSocketPythonGameMode() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
SOCKETPYTHON_API UClass* Z_Construct_UClass_ASocketPythonGameMode();
SOCKETPYTHON_API UClass* Z_Construct_UClass_ASocketPythonGameMode_NoRegister();
UPackage* Z_Construct_UPackage__Script_SocketPython();
// End Cross Module References

// Begin Class ASocketPythonGameMode
void ASocketPythonGameMode::StaticRegisterNativesASocketPythonGameMode()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(ASocketPythonGameMode);
UClass* Z_Construct_UClass_ASocketPythonGameMode_NoRegister()
{
	return ASocketPythonGameMode::StaticClass();
}
struct Z_Construct_UClass_ASocketPythonGameMode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering HLOD WorldPartition DataLayers Transformation" },
		{ "IncludePath", "SocketPythonGameMode.h" },
		{ "ModuleRelativePath", "SocketPythonGameMode.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ASocketPythonGameMode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_ASocketPythonGameMode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AGameModeBase,
	(UObject* (*)())Z_Construct_UPackage__Script_SocketPython,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ASocketPythonGameMode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ASocketPythonGameMode_Statics::ClassParams = {
	&ASocketPythonGameMode::StaticClass,
	"Game",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x008802ACu,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ASocketPythonGameMode_Statics::Class_MetaDataParams), Z_Construct_UClass_ASocketPythonGameMode_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_ASocketPythonGameMode()
{
	if (!Z_Registration_Info_UClass_ASocketPythonGameMode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ASocketPythonGameMode.OuterSingleton, Z_Construct_UClass_ASocketPythonGameMode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ASocketPythonGameMode.OuterSingleton;
}
template<> SOCKETPYTHON_API UClass* StaticClass<ASocketPythonGameMode>()
{
	return ASocketPythonGameMode::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(ASocketPythonGameMode);
ASocketPythonGameMode::~ASocketPythonGameMode() {}
// End Class ASocketPythonGameMode

// Begin Registration
struct Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_DemoSimulationUE5_SocketPython_Source_SocketPython_SocketPythonGameMode_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ASocketPythonGameMode, ASocketPythonGameMode::StaticClass, TEXT("ASocketPythonGameMode"), &Z_Registration_Info_UClass_ASocketPythonGameMode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ASocketPythonGameMode), 4079836712U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_DemoSimulationUE5_SocketPython_Source_SocketPython_SocketPythonGameMode_h_2242192739(TEXT("/Script/SocketPython"),
	Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_DemoSimulationUE5_SocketPython_Source_SocketPython_SocketPythonGameMode_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Vag_Programs_UnrealEngine_UE_5_5_DemoSimulationUE5_SocketPython_Source_SocketPython_SocketPythonGameMode_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
