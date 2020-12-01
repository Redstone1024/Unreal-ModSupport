// Copyright Epic Games, Inc. All Rights Reserved.

#include "ModPackager.h"
#include "ModSupportEditor.h"
#include "ModSupportEditorCommands.h"
#include "ModSupportEditorStyle.h"
#include "ModSupportEditorLog.h"
#include "Editor.h"
#include "Widgets/SWindow.h"
#include "Widgets/SWidget.h"
#include "Interfaces/IPluginManager.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Editor/UATHelper/Public/IUATHelperModule.h"
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"

#include "FileHelpers.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"

#define LOCTEXT_NAMESPACE "ModPackager"

FModPackager::FModPackager()
{
}

FModPackager::~FModPackager()
{
}

void FModPackager::OpenPluginPackager(TSharedRef<IPlugin> Plugin)
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

	FString DefaultDirectory = FPaths::ConvertRelativePathToFull(Plugin->GetBaseDir());
	FString OutputDirectory;

	// Prompt the user to save all dirty packages. We'll ensure that if any packages from the mod that the user wants to
	// package are dirty that they will not be able to save them.

	if (!IsAllContentSaved(Plugin))
	{
		FEditorFileUtils::SaveDirtyPackages( true, true,  true);
	}

	if (IsAllContentSaved(Plugin))
	{
		void* ParentWindowWindowHandle = nullptr;
		IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
		const TSharedPtr<SWindow>& MainFrameParentWindow = MainFrameModule.GetParentWindow();
		if (MainFrameParentWindow.IsValid() && MainFrameParentWindow->GetNativeWindow().IsValid())
		{
			ParentWindowWindowHandle = MainFrameParentWindow->GetNativeWindow()->GetOSWindowHandle();
		}

		if (DesktopPlatform->OpenDirectoryDialog(ParentWindowWindowHandle, LOCTEXT("SelectOutputFolderTitle", "Select Mod output directory:").ToString(), DefaultDirectory, OutputDirectory))
		{
			PackagePlugin(Plugin, OutputDirectory);
		}
	}
	else
	{
		FText PackageModError = FText::Format(LOCTEXT("PackageModError_UnsavedContent", "You must save all assets in {0} before you can share it."),
			FText::FromString(Plugin->GetName()));

		FMessageDialog::Open(EAppMsgType::Ok, PackageModError);
	}
}

bool FModPackager::IsAllContentSaved(TSharedRef<IPlugin> Plugin)
{
	bool bAllContentSaved = true;

	TArray<UPackage*> UnsavedPackages;
	FEditorFileUtils::GetDirtyContentPackages(UnsavedPackages);
	FEditorFileUtils::GetDirtyWorldPackages(UnsavedPackages);

	if (UnsavedPackages.Num() > 0)
	{
		FString PluginBaseDir = Plugin->GetBaseDir();

		for (UPackage* Package : UnsavedPackages)
		{
			FString PackageFilename;
			if (FPackageName::TryConvertLongPackageNameToFilename(Package->GetName(), PackageFilename))
			{
				if (PackageFilename.Find(PluginBaseDir) == 0)
				{
					bAllContentSaved = false;
					break;
				}
			}
		}
	}

	return bAllContentSaved;
}

void FModPackager::PackagePlugin(TSharedRef<class IPlugin> Plugin, const FString& OutputDirectory)
{
	FString PackageCofnig;
	FString PackageCofnigTemplate;
	PackageCofnigTemplate = IPluginManager::Get().FindPlugin(TEXT("ModSupport"))->GetBaseDir() / TEXT("Resources") / TEXT("ModPackageCofnig.json");

	if (!FFileHelper::LoadFileToString(PackageCofnig, *PackageCofnigTemplate))
	{
		UE_LOG(LogModSupportEditor, Error, TEXT("Failed to load configuration template"));
		return;
	}

	PackageCofnig = PackageCofnig.Replace(TEXT("%%%PluginName%%%"), *Plugin->GetName());
	PackageCofnig = PackageCofnig.Replace(TEXT("%%%PluginContentDir%%%"), *Plugin->GetMountedAssetPath());

#if PLATFORM_WINDOWS
	PackageCofnig = PackageCofnig.Replace(TEXT("%%%TargetPlatform%%%"), TEXT("WindowsNoEditor"));
#elif PLATFORM_MAC
	PackageCofnig = PackageCofnig.Replace(TEXT("%%%TargetPlatform%%%"), TEXT("MacNoEditor"));
#elif PLATFORM_LINUX
	PackageCofnig = PackageCofnig.Replace(TEXT("%%%TargetPlatform%%%"), TEXT("LinuxNoEditor"));
#else
	PackageCofnig = PackageCofnig.Replace(TEXT("%%%TargetPlatform%%%"), TEXT("AllDesktop"));
#endif

	PackageCofnig = PackageCofnig.Replace(TEXT("%%%OutputDirectory%%%"), *OutputDirectory);

	FString PackageCofnigSavePath;
	PackageCofnigSavePath = FPaths::ProjectSavedDir() / TEXT("ModInfo") / TEXT("ModPackageCofnig.json");

	if (!FFileHelper::SaveStringToFile(PackageCofnig, *PackageCofnigSavePath))
	{
		UE_LOG(LogModSupportEditor, Error, TEXT("Failed to save configuration"));
		return;
	}

#if PLATFORM_WINDOWS
	PackageCofnigSavePath = PackageCofnigSavePath.Replace(TEXT("/"), TEXT("\\"));
#endif

	FText OptTitle = LOCTEXT("PackageCofnigDialog", "Saved packaging configuration file");
	FText Message = FText::FromString(PackageCofnigSavePath);
	FMessageDialog::Open(EAppMsgType::Ok, Message, &OptTitle);
	UE_LOG(LogModSupportEditor, Display, TEXT("Saved packaging configuration file to %s"), *PackageCofnigSavePath);
}

void FModPackager::FindAvailableGameMods(TArray<TSharedRef<IPlugin>>& OutAvailableGameMods)
{
	OutAvailableGameMods.Empty();

	for (TSharedRef<IPlugin> Plugin : IPluginManager::Get().GetDiscoveredPlugins())
	{
		if (Plugin->GetLoadedFrom() == EPluginLoadedFrom::Project && Plugin->GetType() == EPluginType::Mod)
		{
			UE_LOG(LogModSupportEditor, Display, TEXT("Adding %s"), *Plugin->GetName());
			OutAvailableGameMods.AddUnique(Plugin);
		}
	}
}

void FModPackager::GeneratePackagerMenuContent_Internal(class FMenuBuilder& MenuBuilder, const TArray<TSharedPtr<FUICommandInfo>>& Commands)
{
	for (TSharedPtr<FUICommandInfo> Command : Commands)
	{
		MenuBuilder.AddMenuEntry(Command, NAME_None, TAttribute<FText>(), TAttribute<FText>(), FSlateIcon(FModSupportEditorStyle::GetStyleSetName(), "ModSupportEditor.Folder"));
	}
}

void FModPackager::GeneratePackagerMenuContent(class FMenuBuilder& MenuBuilder)
{
	TArray<TSharedRef<IPlugin>> AvailableGameMods;
	FindAvailableGameMods(AvailableGameMods);

	TArray<TSharedPtr<FUICommandInfo>> Commands;

	GeneratePackagerMenuContent_Internal(MenuBuilder, ModCommands);
}

TSharedRef<SWidget> FModPackager::GeneratePackagerComboButtonContent()
{
	// Regenerate the game mod commands
	TArray<TSharedRef<IPlugin>> AvailableGameMods;
	FindAvailableGameMods(AvailableGameMods);

	GetAvailableModCommands(AvailableGameMods);

	// Regenerate the action list
	TSharedPtr<FUICommandList> GameModActionsList = MakeShareable(new FUICommandList);

	for (int32 Index = 0; Index < ModCommands.Num(); ++Index)
	{
		GameModActionsList->MapAction(
			ModCommands[Index],
			FExecuteAction::CreateRaw(this, &FModPackager::OpenPluginPackager, AvailableGameMods[Index]),
			FCanExecuteAction()
		);
	}

	// Show the drop down menu
	const bool bShouldCloseWindowAfterMenuSelection = true;
	FMenuBuilder MenuBuilder(bShouldCloseWindowAfterMenuSelection, GameModActionsList);

	MenuBuilder.BeginSection(NAME_None, LOCTEXT("PackageMod", "Share..."));
	{
		GeneratePackagerMenuContent_Internal(MenuBuilder, ModCommands);
	}
	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}

void FModPackager::GetAvailableModCommands(const TArray<TSharedRef<IPlugin>>& AvailableMod)
{
	if (ModCommands.Num() > 0)
	{
		// Unregister UI Commands
		FModSupportEditorCommands::Get().UnregisterModCommands(ModCommands);
	}
	ModCommands.Empty(AvailableMod.Num());

	ModCommands = FModSupportEditorCommands::Get().RegisterModCommands(AvailableMod);
}

#undef LOCTEXT_NAMESPACE