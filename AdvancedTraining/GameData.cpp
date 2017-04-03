#include "GameData.h"
#include "helpers.h"
SaveData::SaveData()
{
	snapshots = new vector<GameSnapshot>();
}

SaveData::~SaveData()
{
	if (snapshots) {
		snapshots->clear();
#ifndef _DEBUG
		//delete snapshots; //Crashes the debugger in VS? Also crashes in release FUCK
#endif
		snapshots = NULL;
	}
}

GameSnapshot SaveData::GetFirstFrame()
{
	return snapshots->at(0);
}

int SaveData::GetNumPlayers()
{
	return header.numPlayers; //GetFirstFrame().cars.size();
}

void SaveData::Clear()
{
	snapshots->clear();
}

string get_safe_name(string filename, int index) {
	if (file_exists(filename + to_string(index) + ".data")) {
		return get_safe_name(filename, index + 1);
	}
	return filename + to_string(index) + ".data";
}


void SaveData::Save(string fileName)
{
	header.version = 2;
	header.numPlayers = GetFirstFrame().cars.size();
	header.frameCount = snapshots->size();
	std::ofstream out(fileName.c_str(), ios::out | ios::trunc | ios::binary);
	write_pod(out, header);
	for (unsigned int i = 0; i < header.frameCount; i++) {
		GameSnapshot snapshot = snapshots->at(i);
		write_pod(out, snapshot.timestamp);
		write_pod(out, snapshot.ball);
		write_pod(out, snapshot.ballSpeed);
		for (unsigned int i = 0; i < header.numPlayers; i++) {
			write_pod(out, snapshot.cars.at(i));
		}
	}

	/*int nameIndex = 0;
	ofstream ballFile;
	ballFile.open(get_safe_name("./bakkesmod/analysis/replay_ball_", 0));
	ofstream carFile;
	carFile.open(get_safe_name("./bakkesmod/analysis/replay_car_", 0));
	for (unsigned int i = 0; i < header.frameCount; i++) 
	{
		GameSnapshot snapshot = snapshots->at(i);
		ballFile << snapshot.timestamp << "," << snapshot.ball.location.X << "," << snapshot.ball.location.Y << "," << snapshot.ball.location.Z << "," << snapshot.ballSpeed << "\n";
		carFile << snapshot.timestamp << "," << snapshot.cars.at(0).data.location.X << "," << snapshot.cars.at(0).data.location.Y << "," << snapshot.cars.at(0).data.location.Z << "\n";
	}
	ballFile.close();
	carFile.close();*/
}

void SaveData::Load(string fileName)
{
	std::ifstream in(fileName.c_str(), ios::binary);
	read_pod(in, header);
	for (unsigned int i = 0; i < header.frameCount; i++) {
		GameSnapshot newSnap;
		read_pod(in, newSnap.timestamp);
		read_pod(in, newSnap.ball);
		read_pod(in, newSnap.ballSpeed);
		for (unsigned int i = 0; i < header.numPlayers; i++) {
			CarData c;
			read_pod(in, c);
			newSnap.cars[i] = c;
		}
		snapshots->push_back(newSnap);
	}
}
