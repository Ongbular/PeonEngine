#include "CorePCH.h"
#include "LocalTimer.h"

void LocalTimer::SetDeltaSecondsThisFrame(const float deltaSeconds)
{
	mTotalSeconds += deltaSeconds;
	mDeltaSeconds = deltaSeconds;
}
