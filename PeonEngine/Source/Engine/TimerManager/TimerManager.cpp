#include "CorePCH.h"
#include "TimerManager.h"

float TimerManager::UpdateDeltaSeconds(const bool bShouldStop)
{
	if (bShouldStop)
	{
		mPrevTime = mClock.now();
		return 0.f;
	}

	mFrameCount++;
	mNowTime = mClock.now();

	const auto deltaNanoseconds = mNowTime - mPrevTime;
	mPrevTime = mNowTime;

	// ������ ������ deltaNanoseconds�� �� ������ �ٲٱ� ���ؼ� 1 x 10^(-9)�� ���մϴ�.
	mDeltaSeconds = static_cast<float>(deltaNanoseconds.count() * 1e-9);
	mElapsedSeconds += mDeltaSeconds;

	if (mElapsedSeconds > 0.5f)
	{
		mFramePerSeconds = static_cast<float>(mFrameCount) / mElapsedSeconds * 2;

		mFrameCount = 0;
		mElapsedSeconds = 0.0;
	}

	return mDeltaSeconds;
}
