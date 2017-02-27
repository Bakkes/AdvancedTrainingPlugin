#include "AdvancedTrainingPlugin.h"
#include "Recorder.h"
#include "PlayBacker.h"
BAKKESMOD_PLUGIN(AdvancedTrainingPlugin, "Advanced training Plugin", "0.2", 0)

GameWrapper* gw;
ConsoleWrapper* cons;
Recorder* currentRecorder;
PlayBacker* playBacker;
float snapshot_interval = (1.0f / 60.0f) * 1000;

long long playback() {
	if (playBacker != NULL)
		playBacker->ApplyFrame();
	return 1;
}

void run_playback() {
	if (!gw->IsInTutorial() || playBacker == NULL || !playBacker->IsPlayingBack())
		return;

	gw->SetTimeout([](GameWrapper* gameWrapper) {
		run_playback();
	}, playback());
}

long long record_snapshot() {
	currentRecorder->RecordFrame();
	return snapshot_interval;
}

void record_callback() {
	if (currentRecorder == NULL || !currentRecorder->IsRecording())
		return;

	gw->SetTimeout([](GameWrapper* gameWrapper) {
		record_callback();
	}, record_snapshot());
}

void StartRecording(Recorder* r) {
	currentRecorder = r;
	currentRecorder->StartRecording();
	
	record_callback();
}

void advancedTrainingPlugin_onCommand(std::vector<std::string> params) 
{
	string command = params.at(0);
	if (!gw->IsInTutorial() && !gw->IsInReplay() && !gw->IsInCustomTraining()) {
		return;
	}

	if (command.compare("record_start") == 0) 
	{ 
		if (currentRecorder != NULL)
			delete currentRecorder;
		if (gw->IsInReplay() || gw->IsInCustomTraining() || gw->IsInTutorial())
		{
			StartRecording(new ServerRecorder(gw, cons));
		}
		else if (gw->IsInTutorial()) {
			StartRecording(new SinglePlayerRecorder(gw, cons));
		}
	}
	else if (command.compare("record_stop") == 0) 
	{
		if (currentRecorder == NULL)
			return;
		currentRecorder->GetRecording().Save("playa.rec");
		currentRecorder->StopRecording();
		delete currentRecorder;
		currentRecorder = NULL;
	}
	else if (command.compare("record_stop_discard") == 0) 
	{
		if (currentRecorder == NULL)
			return;
		currentRecorder->StopRecording();
		delete currentRecorder;
		currentRecorder = NULL;
	}
	else if (command.compare("record_load") == 0) 
	{
		if (!gw->IsInTutorial())
			return;

		if (playBacker == NULL)
			playBacker = new PlayBacker(gw, cons);

		gw->GetGameEventAsTutorial().KickBots();
		playBacker->LoadRecording("playa.rec");
		playBacker->StartPlayback();
		run_playback();
	}
	else if (command.compare("record_playback_stop") == 0) 
	{
		if (!gw->IsInTutorial())
			return;

		if (playBacker != NULL)
			playBacker->StopPlayback();
		gw->GetGameEventAsTutorial().KickBots();
	}
	else if (command.compare("woosh") == 0) 
	{
		if (!gw->IsInTutorial())
			return;
		Vector angularVel = gw->GetLocalCar().GetAngularVelocity();
		cons->log(to_string(angularVel.X) + ", " + to_string(angularVel.Y) + ", " + to_string(angularVel.Z));
		gw->GetLocalCar().SetAngularVelocity(Vector(1200, 500, 1200));
		
		gw->SetTimeout([](GameWrapper* gw) {
			if (!gw->IsInTutorial())
				return;
			Vector angularVel = gw->GetLocalCar().GetAngularVelocity();
			cons->log(to_string(angularVel.X) + ", " + to_string(angularVel.Y) + ", " + to_string(angularVel.Z)); }, 10);
	}
	//	recorderEnabled = false;
	//	save_replay();
	//	currentSaveData.Clear();
	
	//else if (command.compare("record_fps") == 0) {
	//	float new_fps = cons->getCvarFloat("record_fps");
	//	snapshot_interval = 1.0f / new_fps;
	//	max_history = (snapshot_interval * new_fps) * 15;
	//	/*rewinderEnabled = true;
	//	recorderEnabled = false;
	//	lastPlaybackTime = gw->GetGameEventAsTutorial().GetSecondsElapsed();
	//	start_playback();*/
	//}
	//else if (command.compare("record_load") == 0) {
	//	reinit();
	//	gw->GetGameEventAsTutorial().KickBots();
	//	load_recording();
	//}
	//else if (command.compare("record_playback_stop") == 0) {
	//	playbackEnabled = false;
	//	gw->GetGameEventAsTutorial().KickBots();
	//	reinit();
	//}
}

void AdvancedTrainingPlugin::onLoad()
{
	gw = gameWrapper;
	cons = console;

	cons->registerNotifier("record_start", advancedTrainingPlugin_onCommand);
	cons->registerNotifier("record_stop", advancedTrainingPlugin_onCommand);
	cons->registerNotifier("record_stop_discard", advancedTrainingPlugin_onCommand);
	cons->registerCvar("record_fps", "60");

	cons->registerNotifier("record_fps", advancedTrainingPlugin_onCommand);
	cons->registerNotifier("record_playback_stop", advancedTrainingPlugin_onCommand);
	cons->registerNotifier("record_load", advancedTrainingPlugin_onCommand);

	cons->registerNotifier("woosh", advancedTrainingPlugin_onCommand);
}

void AdvancedTrainingPlugin::onUnload()
{
}
