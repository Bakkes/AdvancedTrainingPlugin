#include "PlayBacker.h"



PlayBacker::PlayBacker(GameWrapper * gwi, ConsoleWrapper* cons)
{
	this->gwi = gwi;
	this->cons = cons;
}



PlayBacker::~PlayBacker()
{
}

void PlayBacker::ApplyFrame()
{
	if (!isPlayingBack)
		return;
	TutorialWrapper tw = gwi->GetGameEventAsTutorial();
	if (abs(vsize(tw.GetBall().GetLocation() - tw.GetGameCar().GetLocation())) < 250) {
		isPlayingBack = false;
		return;
	}

	float currentTimeInMs = tw.GetSecondsElapsed();
	float totalElapsed = currentTimeInMs - replayStartTime;

	vector<GameSnapshot>* ss = recording.snapshots;
	GameSnapshot currentSnapshot = ss->at(currentReplayTick);
	if (currentReplayTick < ss->size() - 2) {
		float timeBetweenFrames = ss->at(currentReplayTick + 1).timestamp - ss->at(currentReplayTick).timestamp;
		while (totalElapsed >= timeBetweenFrames + totalFramesElapsed && currentReplayTick < ss->size() - 2) {
			currentReplayTick++;
			currentSnapshot = ss->at(currentReplayTick);
			totalFramesElapsed += timeBetweenFrames;
			timeBetweenFrames = ss->at(currentReplayTick + 1).timestamp - ss->at(currentReplayTick).timestamp;
		}
	}
	if (currentReplayTick < ss->size() - 2) 
	{
		GameSnapshot nextSnapshot = ss->at(currentReplayTick + 1);
		float frameDiff = nextSnapshot.timestamp - currentSnapshot.timestamp;
		float timeElapsed = totalElapsed - totalFramesElapsed;

		ActorData interpedBall = interp(currentSnapshot.ball, nextSnapshot.ball, frameDiff, timeElapsed);
		interpedBall.apply(tw.GetBall());
		for (unsigned int i = 0; i < currentSnapshot.cars.size(); i++) 
		{
			CarWrapper car = tw.GetPlayers().Get(i + 1);
			CarData currentCar = currentSnapshot.cars.at(i);
			CarData nextCar = nextSnapshot.cars.at(i);
			ActorData interpedCar = interp(currentCar.data, nextCar.data, frameDiff, timeElapsed);
			interpedCar.apply(car);
		}
	}
	
}

void PlayBacker::StartPlayback()
{
	TutorialWrapper tw = gwi->GetGameEventAsTutorial();
	isPlayingBack = true;
	for (unsigned int i = 0; i < recording.GetFirstFrame().cars.size(); i++) {
		tw.SpawnCar();
	}
	replayStartTime = tw.GetSecondsElapsed();
	totalFramesElapsed = 0.0f;
	currentReplayTick = 0;
}

void PlayBacker::StopPlayback()
{
	isPlayingBack = false;
}

bool PlayBacker::IsPlayingBack()
{
	return isPlayingBack;
}

void PlayBacker::LoadRecording(string fileName)
{
	recording.Clear();
	recording.Load(fileName);
}

SaveData PlayBacker::GetRecording()
{
	return recording;
}
