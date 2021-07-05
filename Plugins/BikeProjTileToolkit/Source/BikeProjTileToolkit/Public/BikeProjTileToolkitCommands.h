// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "BikeProjTileToolkitStyle.h"

class FBikeProjTileToolkitCommands : public TCommands<FBikeProjTileToolkitCommands>
{
public:

	FBikeProjTileToolkitCommands()
		: TCommands<FBikeProjTileToolkitCommands>(TEXT("BikeProjTileToolkit"), NSLOCTEXT("Contexts", "BikeProjTileToolkit", "BikeProjTileToolkit Plugin"), NAME_None, FBikeProjTileToolkitStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};