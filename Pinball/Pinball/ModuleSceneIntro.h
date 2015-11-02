#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"
#include "Animation.h"

struct activableBodies;
class PhysBody;

struct lightSwitch
{
	lightSwitch() { sensor = NULL; lights_on = 0; extraBall = false; scoreGiven = 0; counter = 0; }
	~lightSwitch()
	{
		p2List_item <SDL_Rect*>* item = lights.getFirst();
		while (item)
		{
			delete item->data;
			item = item->next;
		}
	}

	PhysBody* sensor;

	p2List<SDL_Rect*> lights;
	int lights_on;
	int scoreGiven;
	bool extraBall;
	p2List_item<PhysBody*>* ball;
	int counter;
};

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
	SDL_Texture* background_lights;
	SDL_Texture* rFlipper;
	SDL_Texture* lFlipper;

	SDL_Texture* ball;
	uint bonus1_fx;
	uint bonus2_fx;
	uint bonus3_fx;
	uint bonusLong_fx;
	uint bonusLong2_fx;

	uint ballBounce_fx;
	uint bell_fx;
	uint ding_fx;
	uint flipper_fx;

	int ballBounceCounter;

	int launcherCount;
	PhysBody* launcher;
	PhysBody* rightFlipper;
	PhysBody* leftFlipper;
	PhysBody* lostBallZone;
	PhysBody* bouncyRight;
	PhysBody* bouncyLeft;
	p2DynArray<PhysBody*> OrangeBouncers;

	p2List<activableBodies> activable;
	p2List<lightSwitch*> lights;

	SDL_Texture* circleTexture;
	Animation circle;


	bool started;
	int score;
	int lastScore;
	int lifes;
	bool draw;
	bool magnet;

	int ballsToAdd;
	bool launcherReady;
	bool ballLaunched;

private:
	void InputCommands();
	void ResizeBalls();
	void Draw();
	void Magnetize();
	void AddBalls();
	void DeleteDeadBalls();

};
