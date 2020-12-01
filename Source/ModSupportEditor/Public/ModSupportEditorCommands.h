// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "ModSupportEditorStyle.h"

class FModSupportEditorCommands : public TCommands<FModSupportEditorCommands>
{
public:

	FModSupportEditorCommands()
		: TCommands<FModSupportEditorCommands>(TEXT("ModSupportEditor"), NSLOCTEXT("Contexts", "ModSupportEditor", "ModSupportEditor Plugin"), NAME_None, FModSupportEditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

	TArray<TSharedPtr<FUICommandInfo>> RegisterModCommands(const TArray<TSharedRef<class IPlugin>>& ModList) const;
	void UnregisterModCommands(TArray<TSharedPtr<FUICommandInfo>>& UICommands) const;

public:
	TSharedPtr< FUICommandInfo > CreateModAction;
	TSharedPtr< FUICommandInfo > PackageModAction;
};