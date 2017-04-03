#pragma once
#include "AdvancedTrainingPlugin.h"
#include "GameData.h"
class PlayBacker
{
protected:
	GameWrapper* gwi;
	ConsoleWrapper* cons;
	bool isPlayingBack;
	SaveData recording;
	float replayStartTime = 0.0f;
	float totalFramesElapsed = 0.0f;
	int currentReplayTick = 0;

public:
	PlayBacker(GameWrapper* gwi, ConsoleWrapper* cons);
	~PlayBacker();
	void ApplyFrame();
	void StartPlayback();
	void StopPlayback();
	bool IsPlayingBack();
	void LoadRecording(string fileName);
	SaveData GetRecording();
};