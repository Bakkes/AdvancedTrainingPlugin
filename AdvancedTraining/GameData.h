#pragma once
#include <fstream>
#include "AdvancedTrainingPlugin.h"
#include "iohelper.h"
static inline float vsize(Vector v) {
	return sqrt(pow(v.X, 2) + pow(v.Y, 2) + pow(v.Z, 2));
}

struct ActorData {
	Vector location;
	Vector velocity;
	Vector angularVelocity;
	Rotator rotation;

	void apply(ActorWrapper aw) {
		aw.SetLocation(location);
		aw.SetVelocity(velocity);
		aw.SetRotation(rotation);
		aw.SetAngularVelocity(angularVelocity);
	}
};

struct CarData {
	ActorData data;
	bool boostActive;
};

struct GameSnapshot {
public:
	map<int, CarData> cars;
	ActorData ball;
	float timestamp;
};

struct SaveHeader {
	unsigned int version = 1;
	size_t numPlayers = 1;
	unsigned int frameCount = 0;
};

static inline ActorData interp(ActorData lhs, ActorData rhs, float hsDiff, float elapsed) {
	if (hsDiff < 0.00001f)
		hsDiff = 0.00001f;

	ActorData nhs;
	Vector snap = Vector(hsDiff);
	Rotator snapR = Rotator(hsDiff);

	nhs.location = lhs.location + (((rhs.location - lhs.location) / snap) * elapsed);
	nhs.velocity = lhs.velocity + (((rhs.velocity - lhs.velocity) / snap) * elapsed);
	nhs.angularVelocity = lhs.angularVelocity + (((rhs.angularVelocity - lhs.angularVelocity) / snap) * elapsed);
	nhs.rotation = lhs.rotation + (((rhs.rotation - lhs.rotation) / snapR) * elapsed); //gets rounded to int prob?
	return nhs;
}

class SaveData {
private:
	SaveHeader h;
public:
	SaveData();
	~SaveData();
	vector<GameSnapshot>* snapshots;
	GameSnapshot GetFirstFrame();
	int GetNumPlayers();
	void Clear();

	void Save(string fileName);
	void Load(string fileName);
};