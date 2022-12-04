#pragma once

class LocalTimer
{
public:
	LocalTimer() = default;
	~LocalTimer() = default;

	void SetDeltaSecondsThisFrame(float deltaSeconds);

	inline float GetDeltaSeconds() const { return mDeltaSeconds; }
	inline float GetTotalSeconds() const { return mTotalSeconds; }

private:
	float mTotalSeconds = 0.f;
	float mDeltaSeconds = 0.f;
};

