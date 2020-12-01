// Copyright Epic Games, Inc. All Rights Reserved.

#include "ModSupportEditor.h"

#include "ModCreator.h"
#include "ModPackager.h"
#include "Misc/MessageDialog.h"
#include "ModSupportEditorStyle.h"
#include "ModSupportEditorCommands.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

#include "LevelEditor.h"

#define LOCTEXT_NAMESPACE "FModSupportEditorModule"

void FModSupportEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	ModCreator = MakeShared<FModCreator>();
	ModPackager = MakeShared<FModPackager>();

	FModSupportEditorStyle::Initialize();
	FModSupportEditorStyle::ReloadTextures();

	FModSupportEditorCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FModSupportEditorCommands::Get().CreateModAction,
		FExecuteAction::CreateRaw(this, &FModSupportEditorModule::CreateModButtonClicked),
		FCanExecuteAction()
	);

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	// Add commands
	{
		FName MenuSection = "FileProject";
		FName ToolbarSection = "Misc";

		// Add creator button to the toolbar
		{
			TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
			ToolbarExtender->AddToolBarExtension(ToolbarSection, EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FModSupportEditorModule::AddModCreatorToolbarExtension));

			LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
		}

		// Add packager button to the toolbar
		{
			TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
			ToolbarExtender->AddToolBarExtension(ToolbarSection, EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FModSupportEditorModule::AddModPackagerToolbarExtension));

			LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
		}
	}
}

void FModSupportEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FModSupportEditorStyle::Shutdown();
	FModSupportEditorCommands::Unregister();
}

void FModSupportEditorModule::CreateModButtonClicked()
{
	if (ModCreator.IsValid())
	{
		ModCreator->OpenNewPluginWizard();
	}
}

void FModSupportEditorModule::AddModCreatorToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FModSupportEditorCommands::Get().CreateModAction);
}

void FModSupportEditorModule::AddModPackagerToolbarExtension(FToolBarBuilder& Builder)
{
	FModPackager* Packager = ModPackager.Get();

	Builder.AddComboButton(FUIAction(),
		FOnGetContent::CreateSP(Packager, &FModPackager::GeneratePackagerComboButtonContent),
		LOCTEXT("PackageMod_Label", "Package Mod"),
		LOCTEXT("PackageMod_Tooltip", "Share and distribute Mod"),
		FSlateIcon(FModSupportEditorStyle::GetStyleSetName(), "ModSupportEditor.PackageModAction")
	);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FModSupportEditorModule, ModSupportEditor)