#pragma once
#include "AdvancedTrainingPlugin.h"
#include "GameData.h"
class Recorder
{
protected:
	GameWrapper* gwi;
	ConsoleWrapper* cons;
	bool isRecording;
	SaveData recording;
	float replayStartTime = 0.0f;
	float lastFrameTime = 0.0f;
public:
	Recorder(GameWrapper* gwi, ConsoleWrapper* cons);
	~Recorder();
	void RecordFrame();
	void StartRecording();
	void StopRecording();
	bool IsRecording();
	SaveData GetRecording();
	virtual float GetSecondsElapsed() = 0;
	virtual GameSnapshot CreateSnapshot() = 0;
};

class ServerRecorder : public Recorder {
public:
	ServerRecorder(GameWrapper* g, ConsoleWrapper* c); 
	GameSnapshot CreateSnapshot() override;
	float GetSecondsElapsed() override;
	CarWrapper GetCar(int idx);
	ServerWrapper GetServerWrapper();
};

class SinglePlayerRecorder : public Recorder {
public:
	SinglePlayerRecorder(GameWrapper* g, ConsoleWrapper* c);
	GameSnapshot CreateSnapshot() override;
	float GetSecondsElapsed() override;
};

class CustomTrainingRecorder : public Recorder {
public:
	CustomTrainingRecorder(GameWrapper* g, ConsoleWrapper* c);
	GameSnapshot CreateSnapshot() override;
	float GetSecondsElapsed() override;
};