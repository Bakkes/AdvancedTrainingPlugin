#pragma once
#pragma comment(lib, "BakkesMod.lib")
#include "plugin\bakkesmodplugin.h"

class AdvancedTrainingPlugin : public bakkesmod::plugin::BakkesModPlugin
{
public:
	virtual void onLoad();
	virtual void onUnload();
};



