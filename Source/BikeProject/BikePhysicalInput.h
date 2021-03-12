// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "HAL/Runnable.h"


/**
 * 
 */
class BIKEPROJECT_API BikePhysicalInput : public FRunnable
{
	FRunnableThread* RunnableThread;
	FSocket* Socket;
	TSharedPtr<FInternetAddr> InternetAddress;
	TArray<uint8>* SpeedsArrayPtr;

public:
	BikePhysicalInput(TArray<uint8>* SpeedsPtr);
	~BikePhysicalInput();

public: // FRunnable Interface

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;


private:

	/** Processes the socket message */
	void ProcessSocketMessage(uint8 InSocketMessage[16]);

private:
	FSocket* ServerUDPSocket;
};
