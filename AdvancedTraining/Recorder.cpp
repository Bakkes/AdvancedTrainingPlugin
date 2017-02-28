#include "Recorder.h"


Recorder::Recorder(GameWrapper * gwi, ConsoleWrapper* cons)
{
	this->gwi = gwi;
	this->cons = cons;
}



Recorder::~Recorder()
{
	
}

void Recorder::RecordFrame()
{
	GameSnapshot snapshot = CreateSnapshot();
	if(snapshot.timestamp >= 0)
		recording.snapshots->push_back(snapshot);
}

void Recorder::StartRecording()
{
	isRecording = true;
	recording.Clear();
	replayStartTime = GetSecondsElapsed();
}

void Recorder::StopRecording()
{
	isRecording = false;
}

bool Recorder::IsRecording()
{
	return isRecording;
}

SaveData Recorder::GetRecording()
{
	return recording;
}

ServerRecorder::ServerRecorder(GameWrapper * g, ConsoleWrapper * c) : Recorder(g, c)
{
}

GameSnapshot ServerRecorder::CreateSnapshot()
{
	ServerWrapper sw = GetServerWrapper();
	ReplayDirectorWrapper rpw = gwi->GetReplayDirector();
	float currentFrameTime = sw.GetSecondsElapsed();
	GameSnapshot shot;
	if (currentFrameTime - lastFrameTime < 0.0001f) {
		shot.timestamp = -1;
		return shot;
	}
	lastFrameTime = sw.GetSecondsElapsed();
	BallWrapper bw = sw.GetBall();
	
	if (bw.IsNull() || bw.IsExploded()) {
		StopRecording();
		shot.timestamp = -1;
		return shot;
	}

	
	shot.ball = { bw.GetLocation(), bw.GetVelocity(), bw.GetAngularVelocity(), bw.GetRotation() };
	shot.timestamp = lastFrameTime - replayStartTime;
	auto cars = sw.GetPRIs();
	int ccount = cars.Count();

	for (unsigned int i = 0; i < ccount; i++) {
		CarWrapper cw = GetCar(i);
		shot.cars[i] = { cw.GetLocation(), cw.GetVelocity(), cw.GetAngularVelocity(), cw.GetRotation(), cw.IsBoostCheap() };
	}

	return shot;
}

float ServerRecorder::GetSecondsElapsed()
{
	ServerWrapper sw = GetServerWrapper();
	return sw.GetSecondsElapsed();
}

CarWrapper ServerRecorder::GetCar(int idx)
{
	ServerWrapper sw = GetServerWrapper();
	if (gwi->IsInReplay())
		return sw.GetPRICar(idx);
	else
		return sw.GetPlayers().Get(idx);
}

ServerWrapper ServerRecorder::GetServerWrapper()
{
	if (gwi->IsInReplay())
		return gwi->GetReplayGameEvent();
	else
		return gwi->GetGameEventAsServer();
}

SinglePlayerRecorder::SinglePlayerRecorder(GameWrapper * g, ConsoleWrapper * c) : Recorder(g, c)
{

}

GameSnapshot SinglePlayerRecorder::CreateSnapshot()
{
	TutorialWrapper tw = gwi->GetGameEventAsTutorial();
	GameSnapshot shot;
	BallWrapper bw = tw.GetBall();
	CarWrapper cw = tw.GetGameCar();
	shot.ball = { bw.GetLocation(), bw.GetVelocity(), bw.GetAngularVelocity(), bw.GetRotation() };
	shot.timestamp = tw.GetSecondsElapsed() - replayStartTime;
	shot.cars[0] = { cw.GetLocation(), cw.GetVelocity(), cw.GetAngularVelocity(), cw.GetRotation(),  cw.IsBoostCheap() };
	return shot;
}

float SinglePlayerRecorder::GetSecondsElapsed()
{
	TutorialWrapper tw = gwi->GetGameEventAsTutorial();
	return tw.GetSecondsElapsed();
}

CustomTrainingRecorder::CustomTrainingRecorder(GameWrapper * g, ConsoleWrapper * c) : Recorder(g, c)
{
}

GameSnapshot CustomTrainingRecorder::CreateSnapshot()
{
	return GameSnapshot();
}

float CustomTrainingRecorder::GetSecondsElapsed()
{
	return 0.0f;
}
