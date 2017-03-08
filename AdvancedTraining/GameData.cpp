#include "GameData.h"

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
		for (unsigned int i = 0; i < header.numPlayers; i++) {
			write_pod(out, snapshot.cars.at(i));
		}
	}
}

void SaveData::Load(string fileName)
{
	std::ifstream in(fileName.c_str(), ios::binary);
	read_pod(in, header);
	for (unsigned int i = 0; i < header.frameCount; i++) {
		GameSnapshot newSnap;
		read_pod(in, newSnap.timestamp);
		read_pod(in, newSnap.ball);
		for (unsigned int i = 0; i < header.numPlayers; i++) {
			CarData c;
			read_pod(in, c);
			newSnap.cars[i] = c;
		}
		snapshots->push_back(newSnap);
	}
}
