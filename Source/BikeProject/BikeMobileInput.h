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
class BIKEPROJECT_API BikeMobileInput : public FRunnable
{
	FRunnableThread* RunnableThread;
	FSocket* Socket;
	FString Address;
	FString ConnectionNum;
	UBikeGameInstance* GameInstanceRef;

public:
	BikeMobileInput(UBikeGameInstance* BikeInstanceRef);
	~BikeMobileInput();

public: // FRunnable Interface

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
};

