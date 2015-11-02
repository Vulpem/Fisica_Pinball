#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

struct activableBodies;
class PhysBody;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	bool GenBackground();

public:
	p2List<PhysBody*> balls;

	SDL_Texture* background_up;
	SDL_Texture* background;
	SDL_Texture* rFlipper;
	SDL_Texture* lFlipper;

	SDL_Texture* circle;
	uint bonus_fx;

	int launcherCount;
	PhysBody* launcher;
	PhysBody* rightFlipper;
	PhysBody* leftFlipper;
	PhysBody* lostBallZone;
	PhysBody* bouncyRight;
	PhysBody* bouncyLeft;
	p2List<activableBodies> activable;

	bool draw;
	bool magnet;

private:
	void InputCommands();
	void ResizeBalls();
	void Draw();
	void Magnetize();

};
