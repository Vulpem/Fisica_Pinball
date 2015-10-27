#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	circle = NULL;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;
	draw = true;
	magnet = false;

	App->renderer->camera.x = App->renderer->camera.y = 0;
	background_up = App->textures->Load("pinball/bg_up.png");
	background = App->textures->Load("pinball/bg.png");
	circle = App->textures->Load("pinball/ballSmall.png"); 
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");

	ret = GenBackground();

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{

	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		PhysBody* circle = App->physics->CreateBall(App->input->GetMouseX(), App->input->GetMouseY());
		circles.add(circle);
		circle->listener = this;
		// TODO 8: Make sure to add yourself as collision callback to the circle you creates
	}
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		draw = !draw;
	}
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		magnet = !magnet;
	}
	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
	{
		App->physics->InvertGravity();
	}

	p2List_item<PhysBody*>* c = circles.getFirst();
	while (c != NULL && magnet)
	{
		int x, y;
		c->data->GetPosition(x, y);
		b2Vec2 force((App->input->GetMouseX() - x) / 50, (App->input->GetMouseY() - y) / 50);
		c->data->body->ApplyForceToCenter(force, true);
		c = c->next;
	}


	// All draw functions ------------------------------------------------------
	if (draw)
	{
		//Drawing background
		App->renderer->Blit(background, 0, 0, NULL);

		c = circles.getFirst();
		while (c != NULL)
		{
			int x, y;
			c->data->GetPosition(x, y);
			App->renderer->Blit(circle, x, y, NULL/*, 1.0f, c->data->GetRotation()*/);
			c = c->next;
		}

		//Background items that go above the ball
		App->renderer->Blit(background_up, 0, 0, NULL);
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	//App->audio->PlayFx(bonus_fx);

}
// TODO 8: Now just define collision callback for the circle and play bonus_fx audio

bool ModuleSceneIntro::GenBackground()
{
	bool ret = true;
	/*	int exterior[50] = {
			232, 650,
			62, 549,
			112, 365,
			150, 364,
			180, 347,
			140, 227,
			171, 215,
			181, 244,
			187, 242,
			164, 170,
			194, 62,
			208, 45,
			231, 31,
			270, 20,
			421, 21,
			452, 26,
			488, 43,
			502, 58,
			510, 86,
			662, 648,
			700, 648,
			543, 0,
			154, 0,
			0, 620,
			104, 649
		};
		bgList.add(App->physics->CreateChain(0, 0, exterior, 50));

		int rightDown[16] = {
			407, 650,
			590, 546,
			546, 364,
			509, 364,
			478, 347,
			517, 237,
			528, 240,
			630, 649
		};
		bgList.add(App->physics->CreateChain(0, 0, rightDown, 16));

		int rightStick[12] = {
			436, 567,
			544, 511,
			521, 387,
			515, 386,
			536, 505,
			434, 557
		};
		bgList.add(App->physics->CreateChain(0, 0, rightStick, 12));

		int leftStick[12] = {
			214, 563,
			108, 507,
			138, 387,
			143, 387,
			116, 502,
			216, 557
		};
		bgList.add(App->physics->CreateChain(0, 0, leftStick, 12));

		int rightBumper[18] = {
			430, 503,
			440, 510,
			491, 485,
			482, 383,
			473, 381,
			466, 392,
			474, 393,
			444, 490,
			433, 491
		};
		bgList.add(App->physics->CreateChain(0, 0, rightBumper, 18));

		int rightBumperBis[8] = {
			447, 489,
			475, 394,
			466, 394,
			433, 489
		};
		bgList.add(App->physics->CreateChain(0, 0, rightBumperBis, 8, 5.0f));

		int leftBumper[20] = {
			213, 509,
			164, 486,
			176, 387,
			181, 382,
			189, 386,
			183, 391,
			209, 484,
			219, 484,
			223, 495,
			221, 507
		};
		bgList.add(App->physics->CreateChain(0, 0, leftBumper, 20));

		int leftBumperBis[8] = {
			203, 483,
			217, 482,
			190, 389,
			181, 394
		};
		bgList.add(App->physics->CreateChain(0, 0, leftBumperBis, 8, 5.0f));

		int fan[38] = {
			292, 243,
			295, 179,
			304, 167,
			316, 161,
			327, 160,
			340, 160,
			353, 162,
			363, 168,
			371, 182,
			373, 242,
			385, 242,
			383, 179,
			374, 161,
			356, 152,
			337, 149,
			321, 149,
			302, 155,
			285, 173,
			282, 243
		};
		bgList.add(App->physics->CreateChain(0, 0, fan, 38));

		int fanInternal[10] = {
			340, 195,
			340, 243,
			327, 243,
			327, 196,
			333, 191
		};
		bgList.add(App->physics->CreateChain(0, 0, fanInternal, 10));

		int blackBox[38] = {
			455, 171,
			417, 117,
			414, 57,
			393, 57,
			392, 40,
			429, 40,
			455, 49,
			470, 60,
			480, 72,
			486, 93,
			502, 179,
			475, 243,
			471, 243,
			497, 174,
			480, 80,
			465, 60,
			432, 55,
			453, 96,
			463, 171
		};
		bgList.add(App->physics->CreateChain(0, 0, blackBox, 38));

		int miniRight[10] = {
			365, 40,
			362, 42,
			362, 55,
			369, 55,
			369, 44
		};
		bgList.add(App->physics->CreateChain(0, 0, miniRight, 10));

		int miniLeft[10] = {
			332, 40,
			329, 42,
			329, 55,
			336, 55,
			336, 44
		};
		bgList.add(App->physics->CreateChain(0, 0, miniLeft, 10));

		int rampZone[38] = {
			215, 139,
			218, 128,
			226, 128,
			226, 125,
			218, 104,
			251, 106,
			242, 125,
			242, 127,
			249, 127,
			250, 130,
			260, 129,
			267, 67,
			306, 56,
			307, 39,
			264, 40,
			251, 44,
			233, 50,
			216, 67,
			205, 139
		};
		bgList.add(App->physics->CreateChain(0, 0, rampZone, 38));

		int orangeLeft[14] = {
			292, 100,
			294, 104,
			300, 107,
			311, 107,
			317, 104,
			319, 100,
			306, 93
		};
		bgList.add(App->physics->CreateChain(0, 0, orangeLeft, 14, 2.0f));


		int orangeCenter[14] = {
			328, 78,
			330, 82,
			336, 85,
			347, 85,
			353, 82,
			355, 78,
			342, 71
		};
		bgList.add(App->physics->CreateChain(0, 0, orangeCenter, 14, 2.0f));

		int orangeRight[14] = {
			360, 100,
			362, 104,
			368, 107,
			379, 107,
			385, 104,
			387, 100,
			374, 93
		};
		bgList.add(App->physics->CreateChain(0, 0, orangeRight, 14, 2.0f));
		*/
		return ret;
}