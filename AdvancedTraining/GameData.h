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
	float ballSpeed;
	float timestamp;
};

struct SaveHeader {
	unsigned int version = 2;
	size_t numPlayers = 1;
	int pov_idx = -1;
	int pov_startframe = 0;
	unsigned int frameCount = 0;
};

static inline ActorData interp(ActorData lhs, ActorData rhs, float hsDiff, float elapsed) {
	if (hsDiff < 0.000000001f) //division by 0 fix lel
		hsDiff = 0.000000001f;

	ActorData nhs;
	Vector snap = Vector(hsDiff);
	Rotator snapR = Rotator(hsDiff);

	nhs.location = lhs.location + (((rhs.location - lhs.location)) * elapsed) / snap;
	nhs.velocity = lhs.velocity + (((rhs.velocity - lhs.velocity)) * elapsed) / snap;
	nhs.angularVelocity = lhs.angularVelocity + (((rhs.angularVelocity - lhs.angularVelocity)) * elapsed) / snap;
	nhs.rotation = lhs.rotation + (((rhs.rotation - lhs.rotation)) * elapsed) / snapR; //gets rounded to int prob?
	return nhs;
}

class SaveData {
public:
	SaveHeader header;
	SaveData();
	~SaveData();
	vector<GameSnapshot>* snapshots;
	GameSnapshot GetFirstFrame();
	int GetNumPlayers();
	void Clear();

	void Save(string fileName);
	void Load(string fileName);
};