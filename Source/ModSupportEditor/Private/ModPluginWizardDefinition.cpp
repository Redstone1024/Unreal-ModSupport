// Copyright Epic Games, Inc. All Rights Reserved.

#include "ModPluginWizardDefinition.h"
#include "ContentBrowserModule.h"
#include "EngineAnalytics.h"
#include "Interfaces/IPluginManager.h"
#include "IContentBrowserSingleton.h"
#include "Algo/Transform.h"
#include "SlateBasics.h"
#include "SourceCodeNavigation.h"

#define LOCTEXT_NAMESPACE "SimpleModPluginWizard"

FModPluginWizardDefinition::FModPluginWizardDefinition()
{
	PluginBaseDir = IPluginManager::Get().FindPlugin(TEXT("ModSupport"))->GetBaseDir();
	BackingTemplate = MakeShareable(new FPluginTemplateDescription(FText(), FText(), TEXT("BaseTemplate"), true, EHostType::Runtime));
	BackingTemplatePath = PluginBaseDir / TEXT("Templates") / BackingTemplate->OnDiskPath;
}

const TArray<TSharedRef<FPluginTemplateDescription>>& FModPluginWizardDefinition::GetTemplatesSource() const
{
	return TemplateDefinitions;
}

void FModPluginWizardDefinition::OnTemplateSelectionChanged(TArray<TSharedRef<FPluginTemplateDescription>> InSelectedItems, ESelectInfo::Type SelectInfo)
{
	SelectedTemplates = InSelectedItems;
}

TArray<TSharedPtr<FPluginTemplateDescription>> FModPluginWizardDefinition::GetSelectedTemplates() const
{
	TArray<TSharedPtr<FPluginTemplateDescription>> SelectedTemplatePtrs;

	for (TSharedRef<FPluginTemplateDescription> Ref : SelectedTemplates)
	{
		SelectedTemplatePtrs.Add(Ref);
	}

	return SelectedTemplatePtrs;
}

void FModPluginWizardDefinition::ClearTemplateSelection()
{
	SelectedTemplates.Empty();
}

bool FModPluginWizardDefinition::HasValidTemplateSelection() const
{
	// A mod should be created even if no templates are actually selected
	return true;
}

bool FModPluginWizardDefinition::CanContainContent() const 
{
	bool bHasContent = SelectedTemplates.Num() == 0;	// if no templates are selected, by default it is a content mod

	if (!bHasContent)
	{
		for (TSharedPtr<FPluginTemplateDescription> Template : SelectedTemplates)
		{
			// If at least one module can contain content, it's a content mod. Otherwise, it's a pure code mod.
			if (Template->bCanContainContent)
			{
				bHasContent = true;
				break;
			}
		}
	}

	return bHasContent;
}

bool FModPluginWizardDefinition::HasModules() const
{
	return false;
}

bool FModPluginWizardDefinition::IsMod() const
{
	return true;
}

void FModPluginWizardDefinition::OnShowOnStartupCheckboxChanged(ECheckBoxState CheckBoxState)
{
}

ECheckBoxState FModPluginWizardDefinition::GetShowOnStartupCheckBoxState() const
{
	return ECheckBoxState();
}

FText FModPluginWizardDefinition::GetInstructions() const
{
	return LOCTEXT("CreateNewModPanel", "Give your new Mod package a name and Click 'Create Mod' to make a new content only Mod package.");
}

TSharedPtr<SWidget> FModPluginWizardDefinition::GetCustomHeaderWidget()
{
	if ( !CustomHeaderWidget.IsValid() )
	{
		FString IconPath;
		GetPluginIconPath(IconPath);

		const FName BrushName(*IconPath);
		const FIntPoint Size = FSlateApplication::Get().GetRenderer()->GenerateDynamicImageResource(BrushName);
		if ((Size.X > 0) && (Size.Y > 0))
		{
			IconBrush = MakeShareable(new FSlateDynamicImageBrush(BrushName, FVector2D(Size.X, Size.Y)));
		}

		CustomHeaderWidget = SNew(SHorizontalBox)
			// Header image
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(4.0f)
			[
				SNew(SBox)
				.WidthOverride(80.0f)
				.HeightOverride(80.0f)
				[
					SNew(SImage)
					.Image(IconBrush.IsValid() ? IconBrush.Get() : nullptr)
				]
			];
	}

	return CustomHeaderWidget;
}

bool FModPluginWizardDefinition::GetPluginIconPath(FString& OutIconPath) const
{
	// Replace this file with your own 128x128 image if desired.
	OutIconPath = BackingTemplatePath / TEXT("Resources/Icon128.png");
	return false;
}

bool FModPluginWizardDefinition::GetTemplateIconPath(TSharedRef<FPluginTemplateDescription> InTemplate, FString& OutIconPath) const
{
	FString TemplateName = InTemplate->Name.ToString();

	OutIconPath = PluginBaseDir / TEXT("Resources");

	if (TemplateToIconMap.Contains(TemplateName))
	{
		OutIconPath /= TemplateToIconMap[TemplateName];
	}
	else
	{
		// Couldn't find a suitable icon to use for this template, so use the default one instead
		OutIconPath /= TEXT("Icon128.png");
	}
	
	return false;
}

FString FModPluginWizardDefinition::GetPluginFolderPath() const
{
	return BackingTemplatePath;
}

EHostType::Type FModPluginWizardDefinition::GetPluginModuleDescriptor() const
{
	return BackingTemplate->ModuleDescriptorType;
}

ELoadingPhase::Type FModPluginWizardDefinition::GetPluginLoadingPhase() const
{
	return BackingTemplate->LoadingPhase;
}

TArray<FString> FModPluginWizardDefinition::GetFoldersForSelection() const
{
	TArray<FString> SelectedFolders;
	SelectedFolders.Add(BackingTemplatePath);	// This will always be a part of the mod plugin

	for (TSharedPtr<FPluginTemplateDescription> Template : SelectedTemplates)
	{
		SelectedFolders.AddUnique(PluginBaseDir / TEXT("Templates") / Template->OnDiskPath);
	}

	return SelectedFolders;
}

void FModPluginWizardDefinition::PluginCreated(const FString& PluginName, bool bWasSuccessful) const
{
	// Override Category to Mod
	if (bWasSuccessful)
	{
		TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(PluginName);
		if (Plugin != nullptr)
		{
			FPluginDescriptor Desc = Plugin->GetDescriptor();
			Desc.Category = "Mod";
			FText UpdateFailureText;
			Plugin->UpdateDescriptor(Desc, UpdateFailureText);
		}
	}
}

#undef LOCTEXT_NAMESPACE
