#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	bool GenBackground();

public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> bgList;

	SDL_Texture* background_up;
	SDL_Texture* background;

	SDL_Texture* circle;
	uint bonus_fx;

	bool draw;
	bool magnet;
};
