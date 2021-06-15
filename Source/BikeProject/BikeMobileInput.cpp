#include "BikeMobileInput.h"

#include "Networking.h"
#include "SocketSubsystem.h"

#include "Misc/Paths.h" 

#include "Engine/Engine.h" 

#include "BikeGameInstance.h"

BikeMobileInput::BikeMobileInput(UBikeGameInstance* BikeInstanceRef)
{
	GameInstanceRef = BikeInstanceRef;

	Socket = nullptr;

	RunnableThread = FRunnableThread::Create(this, TEXT("MobileSocketTask"), 0, TPri_BelowNormal);
}

BikeMobileInput::~BikeMobileInput()
{
	Stop();

	if (Socket != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("deleting socket"));
		delete Socket;
		Socket = nullptr;
	}

	delete RunnableThread;
	RunnableThread = nullptr;
}

bool BikeMobileInput::Init()
{
	bool canBind = false;
	TSharedRef<FInternetAddr> localIp = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, canBind);

	if (localIp->IsValid())
	{
		Address = localIp->ToString(false); // if you want to append the port (true) or not (false).
	}
	int32 Port = 6124;
	FIPv4Address ip;
	FIPv4Address::Parse(Address, ip);

	Socket = FUdpSocketBuilder(TEXT("Mobile Socket")).BoundToAddress(ip).BoundToPort(Port).Build();

	return (Socket != nullptr);
}

uint32 BikeMobileInput::Run()
{
	bool WaitingConf = true;

	if (Socket != nullptr)
	{
		while (Socket != nullptr && WaitingConf)
		{

			int32 BufferSize = 1;
			int32 BytesRead = 0;
			uint8 Response[1];

			if (Socket->Recv(Response, BufferSize, BytesRead))
			{
				char TempChar = (char)Response[0];
				ClientAddress = "";
				ClientAddress.AppendChar(TempChar);
				WaitingConf = false;
			}

			if (WaitingConf)
			{
				GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Green, TEXT("Waiting for conf"), true);
				// Sleep to reduce usage of system resources(nearly delta time).
				FPlatformProcess::Sleep(0.03F);
			}
		}
		GameInstanceRef->SetMobileState(true);
		// Continue updating the device while possible...
		while (Socket != nullptr)
		{
			int32 BufferSize = 1;
			int32 BytesRead = 0;
			uint8 Response[1];

			if (Socket->Recv(Response, BufferSize, BytesRead))
			{
				// Cast to BikeInstance here
			}

			// Sleep to reduce usage of system resources(nearly delta time).
			FPlatformProcess::Sleep(0.03F);
		}
	}
	return 0;
}

void BikeMobileInput::Stop()
{
	// This will make the socket connection state different then SCS_Connected and
	// It will break the while loop in Run method.
	if (Socket != nullptr)
	{
		Socket->Close();
	}
}
