// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FModSupportEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	// When the Create Button is clicked
	void CreateModButtonClicked();

	/** Adds the plugin creator as a new toolbar button */
	void AddModCreatorToolbarExtension(FToolBarBuilder& Builder);

	/** Adds the plugin packager as a new toolbar button */
	void AddModPackagerToolbarExtension(FToolBarBuilder& Builder);

private:

	TSharedPtr<class FModCreator> ModCreator;
	TSharedPtr<class FModPackager> ModPackager;
	TSharedPtr<class FUICommandList> PluginCommands;
};
