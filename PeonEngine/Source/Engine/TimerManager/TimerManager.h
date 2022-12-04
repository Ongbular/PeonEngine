#pragma once

class TimerManager
{
public:
	TimerManager() = default;
	TimerManager(const TimerManager&) = delete;
	TimerManager(TimerManager&&) = delete;
	TimerManager& operator=(const TimerManager&) = delete;
	TimerManager& operator=(TimerManager&&) = delete;
	~TimerManager() = default;

	float UpdateDeltaSeconds(bool bShouldStop);

	inline float GetDeltaSeconds() const { return mDeltaSeconds; }
	inline float GetFps() const { return mFramePerSeconds; }

private:
	uint32 mFrameCount{};
	float mFramePerSeconds{};
	float mDeltaSeconds{};
	float mElapsedSeconds{};

	high_resolution_clock mClock{};
	time_point<steady_clock> mPrevTime{};
	time_point<steady_clock> mNowTime{};

	bool mbPaused = false;
};

