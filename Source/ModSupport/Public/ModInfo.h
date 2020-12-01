#pragma once

#include "CoreMinimal.h"
#include "ModInfo.generated.h"

USTRUCT(BlueprintType, Category = "ModSupport|ModInfo")
struct MODSUPPORT_API FModInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "ModSupport|ModInfo")
	FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "ModSupport|ModInfo")
	FString ContentDir;

	UPROPERTY(BlueprintReadOnly, Category = "ModSupport|ModInfo")
	FString VirtualMountPoint;

	UPROPERTY(BlueprintReadOnly, Category = "ModSupport|ModInfo")
	int32 Version;

	UPROPERTY(BlueprintReadOnly, Category = "ModSupport|ModInfo")
	FString VersionName;

	UPROPERTY(BlueprintReadOnly, Category = "ModSupport|ModInfo")
	FString FriendlyName;

	UPROPERTY(BlueprintReadOnly, Category = "ModSupport|ModInfo")
	FString Description;

	UPROPERTY(BlueprintReadOnly, Category = "ModSupport|ModInfo")
	FString Category;

	UPROPERTY(BlueprintReadOnly, Category = "ModSupport|ModInfo")
	FString CreatedBy;

	UPROPERTY(BlueprintReadOnly, Category = "ModSupport|ModInfo")
	FString CreatedByURL;

	UPROPERTY(BlueprintReadOnly, Category = "ModSupport|ModInfo")
	FString DocsURL;

	UPROPERTY(BlueprintReadOnly, Category = "ModSupport|ModInfo")
	FString MarketplaceURL;

	UPROPERTY(BlueprintReadOnly, Category = "ModSupport|ModInfo")
	FString SupportURL;

	UPROPERTY(BlueprintReadOnly, Category = "ModSupport|ModInfo")
	FString EngineVersion;

	UPROPERTY(BlueprintReadOnly, Category = "ModSupport|ModInfo")
	FString ParentPluginName;

	UPROPERTY(BlueprintReadOnly, Category = "ModSupport|ModInfo")
	bool bIsBetaVersion;

	UPROPERTY(BlueprintReadOnly, Category = "ModSupport|ModInfo")
	bool bIsExperimentalVersion;

	UPROPERTY(BlueprintReadOnly, Category = "ModSupport|ModInfo")
	bool bIsHidden;

	UPROPERTY(BlueprintReadOnly, Category = "ModSupport|ModInfo")
	TArray<FString> PluginsRequire;
};
