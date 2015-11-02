#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModuleWindow.h"
#include "Animation.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	ball = NULL;
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
	ballsToAdd = 1;
	launcherReady = true;
	ballLaunched = true;
	started = false;
	lifes = 3;

	App->renderer->camera.x = App->renderer->camera.y = 0;
	background_up = App->textures->Load("pinball/bg_up.png");
	background_lights = App->textures->Load("pinball/bg_lights.png");
	background = App->textures->Load("pinball/bg.png");
	ball = App->textures->Load("pinball/ballSmall.png");
	rFlipper = App->textures->Load("pinball/rFlipper.png");
	lFlipper = App->textures->Load("pinball/lFlipper.png");
	circleTexture = App->textures->Load("pinball/circle.png");


	bonus1_fx = App->audio->LoadFx("pinball/bonus.wav");
	bonus2_fx = App->audio->LoadFx("pinball/bonus1.wav");
	bonus3_fx = App->audio->LoadFx("pinball/bonus2.wav");
	bonusLong_fx = App->audio->LoadFx("pinball/long_bonus.wav");
	bonusLong2_fx = App->audio->LoadFx("pinball/long_bonus2.wav");

	ballBounce_fx = App->audio->LoadFx("pinball/ball_bounce.wav");
	bell_fx = App->audio->LoadFx("pinball/bell.wav");
	ding_fx = App->audio->LoadFx("pinball/ding_short.wav");
	flipper_fx = App->audio->LoadFx("pinball/flipper.wav");

	SDL_Rect frame1; frame1.x = 0; frame1.y = 0; frame1.h = 40; frame1.w = 90;
	SDL_Rect frame2(frame1); frame2.y += 40;
	SDL_Rect frame3(frame2); frame3.y += 40;
	SDL_Rect frame4(frame3); frame4.y += 40;
	SDL_Rect frame5(frame4); frame5.y += 40;
	circle.frames.PushBack(frame1);
	circle.frames.PushBack(frame2);
	circle.frames.PushBack(frame3);
	circle.frames.PushBack(frame4);
	circle.frames.PushBack(frame5);
	circle.speed = 0.1f;
	circle.loop = true;

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
	char title[50];
	sprintf_s(title, "Lifes: %d Score: %06d Last Score: %06d", lifes, score, score);
	App->window->SetTitle(title);

	InputCommands();

	ResizeBalls();

	if (magnet)
	{
		Magnetize();
	}

	if (started && balls.count() == 0)
	{
		lifes--;
		started = false;
		ballsToAdd = 1;
	}
	if (lifes == 0)
	{
		lastScore = score;
		score = 0;
		lifes = 3;
	}

	Draw();

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::Magnetize()
{
	p2List_item<PhysBody*>* c = balls.getFirst();
	while (c != NULL && magnet)
	{
		int x, y;
		c->data->GetPosition(x, y);
		b2Vec2 force((App->input->GetMouseX() - x) / 50.0f, (App->input->GetMouseY() - y) / 50.0f);
		c->data->body->ApplyForceToCenter(force, true);
		c = c->next;
	}
}

void ModuleSceneIntro::ResizeBalls()
{
	p2List_item<PhysBody*>* c = balls.getFirst();
	while (c != NULL)
	{
		int x, y;
		int windowW, windowH;
		c->data->GetPosition(x, y);
		App->window->GetSize(windowW, windowH);
		float scale = (float)y / (float)windowH + 1.0f;

c->data->Resize(scale);
c = c->next;
	}
}

void ModuleSceneIntro::Draw()
{
	// All draw functions ------------------------------------------------------
	if (draw)
	{
		//Drawing background
		App->renderer->Blit(background, 0, 0, NULL);

		//Drawing pertinent lights
		p2List_item<lightSwitch*>* currentLight = lights.getFirst();
		while (currentLight)
		{
			p2List_item<SDL_Rect*>* currentRect = currentLight->data->lights.getFirst();
			for (int n = 0; n < currentLight->data->lights_on && n < currentLight->data->lights.count(); n++)
			{
				App->renderer->Blit(background_lights, currentRect->data->x, currentRect->data->y, currentRect->data);
				currentRect = currentRect->next;
			}
			if (currentLight->data->counter < 105)
			{
				currentLight->data->counter++;
			}
			currentLight = currentLight->next;
		}
		//Drawing circle animation
		App->renderer->Blit(circleTexture, 290, 114, &circle.GetCurrentFrame());

		//Drawing balls
		p2List_item<PhysBody*>* c = balls.getFirst();
		while (c != NULL)
		{
			int x, y;
			c->data->GetPosition(x, y);
			App->renderer->Blit(ball, x, y, NULL, 0.0f, 0, 0, 0, c->data->scale);
			c = c->next;
		}

		int x; int y;
		rightFlipper->GetPosition(x, y);
		App->renderer->Blit(rFlipper, x - 79, y - 10, NULL, 0.0f, RADTODEG * rightFlipper->body->GetAngle(), 79, 10);

		leftFlipper->GetPosition(x, y);
		App->renderer->Blit(lFlipper, x - 14, y - 9, NULL, 0.0f, RADTODEG * leftFlipper->body->GetAngle(), 14, 9);

		//Background items that go above the ball
		App->renderer->Blit(background_up, 0, 0, NULL);

	}
}

void ModuleSceneIntro::InputCommands()
{
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		ballsToAdd++;
	}
	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		PhysBody* circle = App->physics->CreateBall(App->input->GetMouseX(), App->input->GetMouseY());
		balls.add(circle);
		circle->listener = this;
	}
	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_REPEAT)
	{

		PhysBody* circle = App->physics->CreateBall(App->input->GetMouseX(), App->input->GetMouseY());
		balls.add(circle);
		circle->listener = this;
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
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		App->audio->PlayFx(flipper_fx);
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		rightFlipper->body->ApplyAngularImpulse(DEGTORAD * 150, true);
	}
	else
	{
		rightFlipper->body->ApplyAngularImpulse(DEGTORAD * -50, true);
	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		leftFlipper->body->ApplyAngularImpulse(DEGTORAD * -150, true);
	}
	else
	{
		leftFlipper->body->ApplyAngularImpulse(DEGTORAD * 50, true);
	}
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		started = true;
	}

	if (launcherReady && !ballLaunched && started)
	{
		launcherReady = false;
		ballLaunched = true;
		b2PrismaticJoint* joint = (b2PrismaticJoint*)launcher->body->GetJointList()->joint;
		joint->EnableMotor(true);
		launcherCount = 0;
		App->audio->PlayFx(bonusLong_fx);
	}
	else if (launcherCount < 50)
	{
		launcherCount++;
	}
	else if (launcherCount == 50)
	{
		b2PrismaticJoint* joint = (b2PrismaticJoint*)launcher->body->GetJointList()->joint;
		joint->EnableMotor(false);
		launcherCount++;
	}
	else if (launcherCount < 100)
	{
		launcherCount++;
	}
	else
	{
		launcherReady = true;
	}
	if (ballBounceCounter <= 10)
	{
		ballBounceCounter++;
	}
}

update_status ModuleSceneIntro::PostUpdate()
{
	AddBalls();

	//Deleting dead balls
	DeleteDeadBalls();
	
	//Activating/deactivating activable bodies
	p2List_item<activableBodies>* item = activable.getFirst();
	while (item)
	{
		if (item->data.shouldBeActive == true && item->data.wall->body->IsActive() == false)
		{
			item->data.wall->body->SetActive(true);
		}
		else if (item->data.shouldBeActive == false && item->data.wall->body->IsActive() == true)
		{
			item->data.wall->body->SetActive(false);
		}
		item = item->next;
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::AddBalls()
{
	if (ballsToAdd > 0 && launcherReady && ballLaunched)
	{
		PhysBody* circle = App->physics->CreateBall(640, 600);
		balls.add(circle);
		circle->listener = this;
		ballsToAdd--;
		ballLaunched = false;
	}
}

void ModuleSceneIntro::DeleteDeadBalls()
{
	p2List_item<PhysBody*>* currentBall = balls.getFirst();
	p2List_item<PhysBody*>* nextBall;
	while (currentBall)
	{
		nextBall = currentBall->next;
		if (currentBall->data->dead == true)
		{
			App->physics->world->DestroyBody(currentBall->data->body);
			balls.del(currentBall);
		}
		currentBall = nextBall;
	}
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	if (bodyB && bodyA)
	{
		if (balls.find(bodyA) != -1 /*|| balls.find(bodyB) != -1*/)
		{
			bool found = false;
			if (started && ballBounceCounter > 10)
			{
				App->audio->PlayFx(ballBounce_fx);
				ballBounceCounter = 0;
			}

			if (/*bodyA == lostBallZone || */bodyB == lostBallZone)
			{
				if (balls.find(bodyA) != -1)
				{
					bodyA->dead = true;
				}
				/*else if (balls.find(bodyB) != -1)
				{
					bodyB->dead = true;
				}*/
				found = true;
			}

			if ((/*bodyA == bouncyLeft || */bodyB == bouncyLeft) && !found)
			{
				b2Vec2 force; force.x = 20; force.y = -30;
				App->physics->Bounce(bodyA, force);
				App->physics->Bounce(bodyB, force);
				found = true;
				App->audio->PlayFx(ding_fx);
			}
			if ((/*bodyA == bouncyRight ||*/ bodyB == bouncyRight) && !found)
			{
				App->audio->PlayFx(ding_fx);
				b2Vec2 force; force.x = -20; force.y = -30;
				App->physics->Bounce(bodyA, force);
				App->physics->Bounce(bodyB, force);
				found = true;
			}
			
			if (bodyB == OrangeBouncers[0] || bodyB == OrangeBouncers[1] || bodyB == OrangeBouncers[2])
			{
				score += 10;
				App->audio->PlayFx(bonus1_fx);
			}

			p2List_item<activableBodies>* item = activable.getFirst();
			while (item && !found)
			{
				if (/*bodyA == item->data.deactivator ||*/ bodyB == item->data.deactivator)
				{
					item->data.Deactivate();
				}
				if (/*bodyA == item->data.activator ||*/ bodyB == item->data.activator)
				{
					item->data.Activate();
				}
				item = item->next;
			}

			p2List_item<lightSwitch*>* currentLight = lights.getFirst();
			while (currentLight && !found)
			{
				if (/*bodyA == currentLight->data->sensor || */bodyB == currentLight->data->sensor)
				{
					if (currentLight->data->counter > 100)
					{
						score += currentLight->data->scoreGiven;
						currentLight->data->lights_on++;
						score += currentLight->data->scoreGiven;
						currentLight->data->counter = 0;
						if (currentLight->data->extraBall && currentLight->data->lights.count()+1 == currentLight->data->lights_on)
						{
							ballsToAdd++;
							currentLight->data->lights_on = 0;
						}
						else
						{
							App->audio->PlayFx(bonus3_fx);
						}
					}
				}
				
				currentLight = currentLight->next;
			}

		}
	}
}

bool ModuleSceneIntro::GenBackground()
{
	bool ret = true;
	int exterior[50] = {
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
	App->physics->CreateChain(exterior, 50);

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
	App->physics->CreateChain(rightDown, 16);

	int rightStick[12] = {
		436, 556,
		544, 511,
		521, 387,
		515, 386,
		536, 505,
		434, 552
	};
	App->physics->CreateChain(rightStick, 12);

	int leftStick[12] = {
		210, 556,
		108, 507,
		138, 387,
		143, 387,
		116, 502,
		218, 552
	};
	App->physics->CreateChain(leftStick, 12);

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
	App->physics->CreateChain(rightBumper, 18);

	int rightBumperBis[8] = {
		447, 489,
		475, 394,
		466, 394,
		433, 489
	};
	//App->physics->CreateChain( rightBumperBis, 8, 2.0f);
	bouncyRight = App->physics->CreatePolygon(rightBumperBis, 8);
	bouncyRight->listener = App->scene_intro;


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
	App->physics->CreateChain(leftBumper, 20);

	int leftBumperBis[8] = {
		203, 483,
		217, 482,
		190, 389,
		181, 394
	};
	//App->physics->CreateChain( leftBumperBis, 8, 2.0f);
	bouncyLeft = App->physics->CreatePolygon(leftBumperBis, 8);
	bouncyLeft->listener = App->scene_intro;

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
	App->physics->CreateChain(fan, 38);

	int fanInternal[10] = {
		340, 195,
		340, 243,
		327, 243,
		327, 196,
		333, 191
	};
	App->physics->CreateChain(fanInternal, 10);

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
	App->physics->CreateChain(blackBox, 38);

	int miniRight[10] = {
		365, 40,
		362, 42,
		362, 55,
		369, 55,
		369, 44
	};
	App->physics->CreateChain(miniRight, 10);

	int miniLeft[10] = {
		332, 40,
		329, 42,
		329, 55,
		336, 55,
		336, 44
	};
	App->physics->CreateChain(miniLeft, 10);

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
	App->physics->CreateChain(rampZone, 38);

	int orangeLeft[14] = {
		292, 100,
		294, 104,
		300, 107,
		311, 107,
		317, 104,
		319, 100,
		306, 93
	};
	OrangeBouncers.PushBack(App->physics->CreateChain(orangeLeft, 14, 2.0f));


	int orangeCenter[14] = {
		328, 78,
		330, 82,
		336, 85,
		347, 85,
		353, 82,
		355, 78,
		342, 71
	};
	OrangeBouncers.PushBack(App->physics->CreateChain(orangeCenter, 14, 2.0f));

	int orangeRight[14] = {
		360, 100,
		362, 104,
		368, 107,
		379, 107,
		385, 104,
		387, 100,
		374, 93
	};
	OrangeBouncers.PushBack(App->physics->CreateChain(orangeRight, 14, 2.0f));

	int bottomTriangle[6] = {
		312, 651,
		323, 633,
		333, 652
	};
	App->physics->CreateChain(bottomTriangle, 6);

	int loseBall[8] = {
		0, 0,
		0, 670,
		700, 670,
		700, 0
	};
	lostBallZone = App->physics->CreateChain(loseBall, 8);
	lostBallZone->listener = App->scene_intro;

	int rFlipper[16] = {
		425, 562,
		359, 590,
		352, 596,
		352, 602,
		357, 606,
		435, 583,
		441, 574,
		435, 562
	};
	rightFlipper = App->physics->CreateFlipper(rFlipper, 430, 570, 1);

	int lFlipper[16] = {
		221, 560,
		291, 592,
		295, 599,
		292, 605,
		285, 606,
		210, 585,
		206, 574,
		211, 563
	};
	leftFlipper = App->physics->CreateFlipper(lFlipper, 220, 570, 0);

	int launcherp[8] = {
		620, 628,
		666, 625,
		671, 645,
		623, 645
	};
	launcher = App->physics->CreateLauncher(launcherp, 8, 690, 650);

	//////Activable walls
	int launcherBlocker[8] = {
		522, 243,
		548, 179,
		553, 209,
		525, 253
	};

	int launcherDeactivator[8] = {
		533, 271,
		568, 269,
		570, 281,
		532, 280
	};

	int launcherActivator[8] = {
		503, 166,
		538, 171,
		533, 153,
		503, 153
	};
	activableBodies launcherBlock(App->physics->CreateChain(launcherBlocker, 8), App->physics->CreateSensor(launcherActivator, 8), App->physics->CreateSensor(launcherDeactivator, 8));
	activable.add(launcherBlock);

	//////Activable walls
	int topLeftBlocker[8] = {
		302, 32,
		306, 15,
		293, 14,
		294, 32
	};

	int topLeftDeactivator[8] = {
		283, 32,
		277, 9,
		259, 11,
		266, 35
	};

	int topLeftActivator[8] = {
		306, 38,
		320, 13,
		333, 14,
		313, 42
	};
	activableBodies topLeftBody(App->physics->CreateChain(topLeftBlocker, 8), App->physics->CreateSensor(topLeftActivator, 8), App->physics->CreateSensor(topLeftDeactivator, 8));
	activable.add(topLeftBody);

	//////Activable walls
	int topRightBlocker[8] = {
		394, 35,
		394, 7,
		408, 7,
		402, 34
	};

	int topRightDeactivator[8] = {
		409, 33,
		419, 7,
		440, 8,
		425, 33
	};

	int topRightActivator[8] = {
		389, 35,
		387, 8,
		380, 7,
		385, 35
	};
	activableBodies topRightBody(App->physics->CreateChain(topRightBlocker, 8), App->physics->CreateSensor(topRightActivator, 8), App->physics->CreateSensor(topRightDeactivator, 8));
	activable.add(topRightBody);

	int leftHolePoints[8] = {
		149, 250,
		128, 219,
		168, 204,
		182, 240
	};
	lightSwitch* leftHole = new lightSwitch;
	leftHole->extraBall = true;
	leftHole->scoreGiven = 100;
	leftHole->sensor = App->physics->CreateSensor(leftHolePoints, 8);
	SDL_Rect* rect = new SDL_Rect;
	rect->x = 181; rect->y = 336; rect->w = 54; rect->h = 42;
	leftHole->lights.add(rect);
	rect = new SDL_Rect;
	rect->x = 177; rect->y = 309; rect->w = 33; rect->h = 24;
	leftHole->lights.add(rect);
	rect = new SDL_Rect;
	rect->x = 172; rect->y = 282; rect->w = 33; rect->h = 24;
	leftHole->lights.add(rect);
	rect = new SDL_Rect;
	rect->x = 163; rect->y = 257; rect->w = 33; rect->h = 24;
	leftHole->lights.add(rect);

	lights.add(leftHole);

	int rightHolePoints[8] = {
		484, 92,
		453, 52,
		424, 52,
		444, 93
	};
	lightSwitch* rightHole = new lightSwitch;
	rightHole->extraBall = true;
	rightHole->scoreGiven = 100;
	rightHole->sensor = App->physics->CreateSensor(rightHolePoints, 8);
	rect = new SDL_Rect;
	rect->x = 438; rect->y = 216; rect->w = 30; rect->h = 25;
	rightHole->lights.add(rect);
	rect = new SDL_Rect;
	rect->x = 456; rect->y = 193; rect->w = 27; rect->h = 20;
	rightHole->lights.add(rect);
	rect = new SDL_Rect;
	rect->x = 465; rect->y = 173; rect->w = 25; rect->h = 20;
	rightHole->lights.add(rect);
	rect = new SDL_Rect;
	rect->x = 464; rect->y = 152; rect->w = 25; rect->h = 20;
	rightHole->lights.add(rect);
	rect = new SDL_Rect;
	rect->x = 461; rect->y = 134; rect->w = 25; rect->h = 20;
	rightHole->lights.add(rect);
	rect = new SDL_Rect;
	rect->x = 458; rect->y = 114; rect->w = 25; rect->h = 20;
	rightHole->lights.add(rect);

	lights.add(rightHole);

	return ret;
}