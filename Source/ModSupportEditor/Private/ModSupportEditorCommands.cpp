// Copyright Epic Games, Inc. All Rights Reserved.

#include "ModSupportEditorCommands.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FModSupportEditorModule"

void FModSupportEditorCommands::RegisterCommands()
{
	UI_COMMAND(CreateModAction, "Create Mod", "Create a new Mod package in a mod plugin", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(PackageModAction, "Package Mod", "Share and distribute your Mod", EUserInterfaceActionType::Button, FInputGesture());
}

TArray<TSharedPtr<FUICommandInfo>> FModSupportEditorCommands::RegisterModCommands(const TArray<TSharedRef<class IPlugin>>& ModList) const
{
	TArray<TSharedPtr<FUICommandInfo>> AvailableModActions;
	AvailableModActions.Reserve(ModList.Num());

	FModSupportEditorCommands* MutableThis = const_cast<FModSupportEditorCommands*>(this);

	for (int32 Index = 0; Index < ModList.Num(); ++Index)
	{
		AvailableModActions.Add(TSharedPtr<FUICommandInfo>());
		TSharedRef<IPlugin> Mod = ModList[Index];

		FString CommandName = "ModEditor_" + Mod->GetName();

		FUICommandInfo::MakeCommandInfo(MutableThis->AsShared(),
			AvailableModActions[Index],
			FName(*CommandName),
			FText::FromString(Mod->GetName()),
			FText::FromString(Mod->GetBaseDir()),
			FSlateIcon(),
			EUserInterfaceActionType::Button,
			FInputGesture());
	}

	return AvailableModActions;
}

void FModSupportEditorCommands::UnregisterModCommands(TArray<TSharedPtr<FUICommandInfo>>& UICommands) const
{
	FModSupportEditorCommands* MutableThis = const_cast<FModSupportEditorCommands*>(this);

	for (TSharedPtr<FUICommandInfo> Command : UICommands)
	{
		FUICommandInfo::UnregisterCommandInfo(MutableThis->AsShared(), Command.ToSharedRef());
	}
}

#undef LOCTEXT_NAMESPACE