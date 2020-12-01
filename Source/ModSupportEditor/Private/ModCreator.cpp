// Copyright Epic Games, Inc. All Rights Reserved.

#include "ModCreator.h"

#include "ModPluginWizardDefinition.h"
#include "Widgets/Docking/SDockTab.h"

// This depends on the Plugin Browser module to work correctly...
#include "IPluginBrowser.h"



#define LOCTEXT_NAMESPACE "FModCreator"

const FName FModCreator::ModSupportEditorPluginCreatorName("ModCreator");

FModCreator::FModCreator()
{
	RegisterTabSpawner();
}

FModCreator::~FModCreator()
{
	UnregisterTabSpawner();
}

void FModCreator::OpenNewPluginWizard(bool bSuppressErrors) const
{
	if (IPluginBrowser::IsAvailable())
	{
		FGlobalTabmanager::Get()->InvokeTab(ModSupportEditorPluginCreatorName);
	}
	else if (!bSuppressErrors)
	{
		FMessageDialog::Open(EAppMsgType::Ok,
			LOCTEXT("PluginBrowserDisabled", "Creating a game mod requires the use of the Plugin Browser, but it is currently disabled."));
	}
}

void FModCreator::RegisterTabSpawner()
{
	FTabSpawnerEntry& Spawner = FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ModSupportEditorPluginCreatorName,
																FOnSpawnTab::CreateRaw(this, &FModCreator::HandleSpawnPluginTab));

	// Set a default size for this tab
	FVector2D DefaultSize(800.0f, 500.0f);
	FTabManager::RegisterDefaultTabWindowSize(ModSupportEditorPluginCreatorName, DefaultSize);

	Spawner.SetDisplayName(LOCTEXT("NewModTabHeader", "Create New Mod Package"));
	Spawner.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FModCreator::UnregisterTabSpawner()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ModSupportEditorPluginCreatorName);
}

TSharedRef<SDockTab> FModCreator::HandleSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	check(IPluginBrowser::IsAvailable());
	return IPluginBrowser::Get().SpawnPluginCreatorTab(SpawnTabArgs, MakeShared<FModPluginWizardDefinition>());
}

#undef LOCTEXT_NAMESPACE