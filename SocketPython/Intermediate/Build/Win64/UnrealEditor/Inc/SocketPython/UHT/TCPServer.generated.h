// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "TCPServer.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef SOCKETPYTHON_TCPServer_generated_h
#error "TCPServer.generated.h already included, missing '#pragma once' in TCPServer.h"
#endif
#define SOCKETPYTHON_TCPServer_generated_h

#define FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_TCPServer_h_14_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FRenderRequestStruct_Statics; \
	SOCKETPYTHON_API static class UScriptStruct* StaticStruct();


template<> SOCKETPYTHON_API UScriptStruct* StaticStruct<struct FRenderRequestStruct>();

#define FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_TCPServer_h_29_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesATCPServer(); \
	friend struct Z_Construct_UClass_ATCPServer_Statics; \
public: \
	DECLARE_CLASS(ATCPServer, AActor, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/SocketPython"), NO_API) \
	DECLARE_SERIALIZER(ATCPServer)


#define FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_TCPServer_h_29_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	ATCPServer(ATCPServer&&); \
	ATCPServer(const ATCPServer&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, ATCPServer); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(ATCPServer); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(ATCPServer) \
	NO_API virtual ~ATCPServer();


#define FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_TCPServer_h_26_PROLOG
#define FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_TCPServer_h_29_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_TCPServer_h_29_INCLASS_NO_PURE_DECLS \
	FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_TCPServer_h_29_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> SOCKETPYTHON_API UClass* StaticClass<class ATCPServer>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Vag_Programs_UnrealEngine_UE_5_5_SocketPython_Source_SocketPython_Public_TCPServer_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
