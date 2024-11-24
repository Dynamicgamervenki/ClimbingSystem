#pragma once

namespace Debug
{
	static void Print(const FString& Msg,FColor Color = FColor::MakeRandomColor(),int32 key = -1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(key,6.0f,Color,Msg);
		}
		UE_LOG(LogTemp,Warning,TEXT("%s"),*Msg);
	}
}
