// Copyright Epic Games, Inc. All Rights Reserved.

#include "BikeProjTileToolkitCommands.h"

#define LOCTEXT_NAMESPACE "FBikeProjTileToolkitModule"

void FBikeProjTileToolkitCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "BikeProjTileToolkit", "Bring up BikeProjTileToolkit window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
