// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericPlatform/GenericPlatformProcess.h" 
#include "Sockets.h"
#include "HAL/Runnable.h"

class BIKEPROJECT_API UBikeGameInstance;

/**
 * 
 */
class BIKEPROJECT_API BikePhysicalInput : public FRunnable
{
	FRunnableThread* RunnableThread;
	FProcHandle ANTProcHandle;
	FSocket* Socket;
	UBikeGameInstance* GameInstanceRef;

public:
	BikePhysicalInput(UBikeGameInstance* BikeInstanceRef);
	~BikePhysicalInput();

public: // FRunnable Interface

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
};
