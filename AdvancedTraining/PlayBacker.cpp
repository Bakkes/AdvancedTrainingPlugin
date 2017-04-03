#include "PlayBacker.h"

PlayBacker::PlayBacker(GameWrapper * gwi, ConsoleWrapper* cons)
{
	this->gwi = gwi;
	this->cons = cons;
}

PlayBacker::~PlayBacker()
{
}

bool closeTo(ActorData ball, ActorData car) {
	return abs(vsize(ball.location - car.location)) < 250;
}

void PlayBacker::ApplyFrame()
{
	TutorialWrapper tw = gwi->GetGameEventAsTutorial();
	if (!isPlayingBack) 
	{
		//if(!tw.IsNull())
		//	tw.GetBall().SetGravityScale(1);
		return;
	}
	
	if (abs(vsize(tw.GetBall().GetLocation() - tw.GetGameCar().GetLocation())) < 400) {
		isPlayingBack = false;
		//tw.GetBall().SetGravityScale(1);
		return;
	}
	//tw.GetBall().SetGravityScale(0);
	float currentTimeInMs = tw.GetSecondsElapsed();
	float totalElapsed = currentTimeInMs - replayStartTime;

	bool newTick = false;

	vector<GameSnapshot>* ss = recording.snapshots;
	GameSnapshot currentSnapshot = ss->at(currentReplayTick);
	if (currentReplayTick < ss->size() - 2) {
		float timeBetweenFrames = ss->at(currentReplayTick + 1).timestamp - ss->at(currentReplayTick).timestamp;
		while (totalElapsed >= timeBetweenFrames + totalFramesElapsed && currentReplayTick < ss->size() - 2) {
			newTick = true;
			currentReplayTick++;
			currentSnapshot = ss->at(currentReplayTick);
			totalFramesElapsed += timeBetweenFrames;
			timeBetweenFrames = ss->at(currentReplayTick + 1).timestamp - currentSnapshot.timestamp;
		}
	}
	if (currentReplayTick < ss->size() - 2) 
	{
		GameSnapshot nextSnapshot = ss->at(currentReplayTick + 1);
		float frameDiff = nextSnapshot.timestamp - currentSnapshot.timestamp;
		float timeElapsed = totalElapsed - totalFramesElapsed;
		int carIdx = 0;
		for (unsigned int i = 0; i < currentSnapshot.cars.size(); i++) 
		{
			if (i == recording.header.pov_idx) 
			{
				if (currentReplayTick == 10) {
					currentSnapshot.cars.at(i).data.apply(tw.GetGameCar());
					//tw.GetGameCar()
				}
			}
			else 
			{
				CarWrapper car = tw.GetPlayers().Get(carIdx + 1);
				CarData currentCar = currentSnapshot.cars.at(i);
				CarData nextCar = nextSnapshot.cars.at(i);
				ActorData interpedCar = interp(currentCar.data, nextCar.data, frameDiff, timeElapsed);
				interpedCar.apply(car);
				//cons->log("Boost " + to_string(currentCar.boostActive));
				if (!car.GetBoost().IsNull() && !car.GetBoost().IsUnlimitedBoost())
				{
					//car.GetBoost().SetActive(currentCar.boostActive);
					//car.GetBoost().SetBoostAmount(100);
					car.GetBoost().SetUnlimitedBoost(true);
				}
				/*if (!car.GetBoost().IsNull())
				{
					car.GetBoost().SetActive(currentCar.boostActive);
				}
				else*/
				{
					car.SetBoostCheap(currentCar.boostActive);
					car.ForceBoost(currentCar.boostActive);
				}
				carIdx++;
			}
			//if(currentReplayTick % 20 == 0)
				//tw.GetBall().StopDoingShit();
		}
		ActorData interpedBall = interp(currentSnapshot.ball, nextSnapshot.ball, frameDiff, timeElapsed);
		//interpedBall.velocity = Vector(0, 0, 0);// interpedBall.velocity * 1.001;
		interpedBall.apply(tw.GetBall());
	}
	
}

void PlayBacker::StartPlayback()
{
	TutorialWrapper tw = gwi->GetGameEventAsTutorial();
	isPlayingBack = true;
	int takeOff = recording.header.pov_idx == -1 ? 0 : 1;
	for (unsigned int i = 0; i < recording.GetFirstFrame().cars.size() - takeOff; i++) {
		tw.SpawnCar();
	}
	replayStartTime = tw.GetSecondsElapsed();
	totalFramesElapsed = 0.0f;
	currentReplayTick = 0;
}

void PlayBacker::StopPlayback()
{
	isPlayingBack = false;
	TutorialWrapper tw = gwi->GetGameEventAsTutorial();
	//if (!tw.IsNull() && !tw.GetBall().IsNull())
		//tw.GetBall().SetGravityScale(1);
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
