// Copyright Epic Games, Inc. All Rights Reserved.

#include "BikeProjTileToolkit.h"
#include "BikeProjTileToolkitStyle.h"
#include "BikeProjTileToolkitCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName BikeProjTileToolkitTabName("BikeProjTileToolkit");

#define LOCTEXT_NAMESPACE "FBikeProjTileToolkitModule"

void FBikeProjTileToolkitModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FBikeProjTileToolkitStyle::Initialize();
	FBikeProjTileToolkitStyle::ReloadTextures();

	FBikeProjTileToolkitCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FBikeProjTileToolkitCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FBikeProjTileToolkitModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FBikeProjTileToolkitModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(BikeProjTileToolkitTabName, FOnSpawnTab::CreateRaw(this, &FBikeProjTileToolkitModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FBikeProjTileToolkitTabTitle", "BikeProjTileToolkit"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FBikeProjTileToolkitModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FBikeProjTileToolkitStyle::Shutdown();

	FBikeProjTileToolkitCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(BikeProjTileToolkitTabName);
}

TSharedRef<SDockTab> FBikeProjTileToolkitModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FBikeProjTileToolkitModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("BikeProjTileToolkit.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
}

void FBikeProjTileToolkitModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(BikeProjTileToolkitTabName);
}

void FBikeProjTileToolkitModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FBikeProjTileToolkitCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FBikeProjTileToolkitCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBikeProjTileToolkitModule, BikeProjTileToolkit)