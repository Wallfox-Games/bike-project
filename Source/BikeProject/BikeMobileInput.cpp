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
		UE_LOG(LogTemp, Warning, TEXT("deleting phone socket"));
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
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("Connected to IP: " + Address + " Port: " + FString::FromInt(Port)), true);

	int AddressIndex;
	ConnectionNum = Address;
	ConnectionNum.FindLastChar('.', AddressIndex);
	ConnectionNum.RemoveAt(0, AddressIndex + 1);

	return (Socket != nullptr);
}

uint32 BikeMobileInput::Run()
{
	bool WaitingConf = true;
	TSharedRef<FInternetAddr> ClientAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	if (Socket != nullptr)
	{
		GameInstanceRef->SetDeviceAddress(ConnectionNum);
		while (Socket != nullptr && WaitingConf)
		{

			int32 BufferSize = 4;
			int32 BytesRead = 0;
			uint8 Response[4];
			
			if (Socket->RecvFrom(Response, BufferSize, BytesRead, *ClientAddress))
			{
				float TempResponse = ((float*)Response)[0];
				int DeviceNum = (int)(TempResponse);

				FString DeviceAddress = ClientAddress->ToString(false);
				int AddressIndex;
				DeviceAddress.FindLastChar('.', AddressIndex);
				DeviceAddress.RemoveAt(0, AddressIndex + 1);

				if (FString::FromInt(DeviceNum) == DeviceAddress)
				{
					ClientAddress->SetPort(Socket->GetPortNo());
					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Client IP: " + ClientAddress->ToString(true)), true);
					GameInstanceRef->SetMobileState(0);
					GameInstanceRef->SetMobileEnabled(true);
					WaitingConf = false;
				}
			}
			if (WaitingConf)
			{
				GEngine->AddOnScreenDebugMessage(2, 0.5f, FColor::Green, TEXT("Waiting for mobile"), true);
				// Sleep to reduce usage of system resources(nearly delta time).
				FPlatformProcess::Sleep(0.03F);
			}
		}	
		// Continue updating the device while possible...
		while (Socket != nullptr)
		{
			int32 BufferSize = 4;
			int32 BytesRead = 0;
			int32 BytesSent = 0;
			uint8 Response[4];

			if (Socket->Recv(Response, BufferSize, BytesRead))
			{
				GEngine->AddOnScreenDebugMessage(2, 10.f, FColor::Green, TEXT("Received: " + FString::SanitizeFloat(((float*)Response)[0])), true);
				if (GameInstanceRef->GetMobileState() != 2)
				{
					GameInstanceRef->SetMobileMessage(((float*)Response)[0]);
				}
				else 
				{
					GameInstanceRef->SetMobileSpeed(((float*)Response)[0]);
				}
			}

			char MessageCharacter = NULL;
			switch (GameInstanceRef->GetMobileState())
			{
			case 0:
				MessageCharacter = 'a';
				break;
			case 1:
				MessageCharacter = 'm';
				break;
			case 2:
				MessageCharacter = 'g';
				break;
			case 3:
				MessageCharacter = 'r';
				break;
			case 4:
				MessageCharacter = 'e';
				break;
			default:
				break;
			}

			FString TempStr = "";
			TempStr.AppendChar(MessageCharacter);
			GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Green, TEXT("Sent: " + TempStr), true);

			Socket->SendTo((uint8*)&MessageCharacter, 1, BytesSent, *ClientAddress);

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
