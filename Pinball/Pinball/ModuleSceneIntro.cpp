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

	LoadAssets();

	GenBackground();

	return ret;
}

// Update
update_status ModuleSceneIntro::Update()
{
	SetTitle();
	
	InputCommands();

	ManageLostBalls();

	ManageLauncher();

	ResizeBalls();

	if (magnet)
	{
		Magnetize();
	}

	Draw();

	return UPDATE_CONTINUE;
}

// Post Update
update_status ModuleSceneIntro::PostUpdate()
{
	AddBalls();

	//Deleting dead balls
	DeleteDeadBalls();

	//Activating/deactivating activable bodies
	ManageActivableBodies();

	return UPDATE_CONTINUE;
}

bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	p2List_item<lightSwitch*>* item = lights.getFirst();
	if (item->data)
	{
		delete item->data;
	}

	return true;
}

/////////////////////////

void ModuleSceneIntro::LoadAssets()
{
	App->renderer->camera.x = App->renderer->camera.y = 0;
	background_up = App->textures->Load("pinball/bg_up.png");
	background_lights = App->textures->Load("pinball/bg_lights.png");
	background = App->textures->Load("pinball/bg.png");
	ball = App->textures->Load("pinball/ballSmall.png");
	rFlipper = App->textures->Load("pinball/rFlipper.png");
	lFlipper = App->textures->Load("pinball/lFlipper.png");
	circleTexture = App->textures->Load("pinball/circle.png");
	orange_bump = App->textures->Load("pinball/orange_bump.png");


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
}

void ModuleSceneIntro::SetTitle()
{
	char title[50];
	sprintf_s(title, "Lifes: %d Score: %06d Last Score: %06d", lifes, score, lastScore);
	App->window->SetTitle(title);
}

void ModuleSceneIntro::InputCommands()
{
	//GAME INPUT COMMANDS
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

	if (!started)
	{
		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
		{
			started = true;
			saveBallCounter = 0;
		}
	}

	//DEBUGGING INPUT COMMANDS
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

void ModuleSceneIntro::ManageLostBalls()
{
	if (started && balls.count() == 0)
	{
		if (saveBallCounter < 550)
		{
			ballsToAdd++;
			saveBallCounter = 0;
		}
		else
		{
			lifes--;
			started = false;
			ballsToAdd = 1;
		}
	}
	if (lifes == 0)
	{
		lastScore = score;
		ballsToAdd = 1;
		score = 0;
		lifes = 3;
		//Turning off all lights that were switched on during the game
		p2List_item<lightSwitch*>* currentLight = lights.getFirst();
		while (currentLight)
		{
			currentLight->data->lights_on = 0;
			currentLight = currentLight->next;
		}
	}
}

void ModuleSceneIntro::ManageLauncher()
{
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

void ModuleSceneIntro::ManageActivableBodies()
{
	p2List_item<activableBodies>* item = activable.getFirst();
	while (item)
	{
		if (item->data.toDeactivate == true)
		{
			if (item->data.wall->body->IsActive() == true)
			{
				item->data.wall->body->SetActive(false);
			}
		}
		else if (item->data.toActivate == true)
		{
			if (item->data.wall->body->IsActive() == false)
			{
				item->data.wall->body->SetActive(true);
			}
		}
		item->data.toDeactivate = false;
		item->data.toActivate = false;
		item = item->next;
	}
}

void ModuleSceneIntro::Draw()
{
	// All draw functions ------------------------------------------------------
	if (draw)
	{
		//Drawing background
		App->renderer->Blit(background, 0, 0, NULL);

		//Drawing "Save Ball" light
		if (saveBallCounter < 550)
		{
			saveBallCounter++;
			SDL_Rect rect;
			rect.x = 310; rect.y = 540; rect.w = 30; rect.h = 40;
			App->renderer->Blit(background_lights, rect.x, rect.y, &rect);
		}

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

		//Drawing Flippers
		int x; int y;
		rightFlipper->GetPosition(x, y);
		App->renderer->Blit(rFlipper, x - 79, y - 10, NULL, 0.0f, RADTODEG * rightFlipper->body->GetAngle(), 79, 10);

		leftFlipper->GetPosition(x, y);
		App->renderer->Blit(lFlipper, x - 14, y - 9, NULL, 0.0f, RADTODEG * leftFlipper->body->GetAngle(), 14, 9);

		//Background items that go above the ball
		App->renderer->Blit(background_up, 0, 0, NULL);

		for (int n = 0; n < 3; n++)
		{
			if (bounceCounters[n] < 10)
			{
				bounceCounters[n]++;
				switch (n)
				{
				case 0: {	App->renderer->Blit(orange_bump, 285, 66, NULL); break; }
				case 1: {	App->renderer->Blit(orange_bump, 320, 43, NULL); break; }
				case 2: {	App->renderer->Blit(orange_bump, 354, 66, NULL); break; }
				}
			}
		}

	}
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

void ModuleSceneIntro::OnCollision(PhysBody* ball, PhysBody* bodyB)
{
	if (bodyB && ball)
	{
		//Making sure one of the colliding objects is a ball
		if (balls.find(ball) != -1)
		{
			bool found = false;
			//Play the sound of the ball bounce, but not too often
			if (started && ballBounceCounter > 10)
			{
				App->audio->PlayFx(ballBounce_fx);
				ballBounceCounter = 0;
			}

			//Marking the balls dead if necessary
			if (bodyB == lostBallZone)
			{
				if (balls.find(ball) != -1)
				{
					ball->dead = true;
				}
				found = true;
			}

			//Bottom triangle bouncers. Playing the sound and applying the correspondant force
			if ((bodyB == bouncyLeft) && !found)
			{
				b2Vec2 force; force.x = 20; force.y = -50;
				App->physics->Bounce(ball, force);
				found = true;
				App->audio->PlayFx(ding_fx);
			}
			if ((bodyB == bouncyRight) && !found)
			{
				App->audio->PlayFx(ding_fx);
				b2Vec2 force; force.x = -20; force.y = -50;
				App->physics->Bounce(ball, force);
				found = true;
			}
			
			//Top orange bouncers. Starting the counter for the convenient graphics, playing the sound
			if (bodyB == OrangeBouncers[0])
			{
				bounceCounters[0] = 0;
				score += 10;
				App->audio->PlayFx(bonus1_fx);
			}
			if (bodyB == OrangeBouncers[1])
			{
				bounceCounters[1] = 0;
				score += 10;
				App->audio->PlayFx(bonus1_fx);
			}
			if (bodyB == OrangeBouncers[2])
			{
				bounceCounters[2] = 0;
				score += 10;
				App->audio->PlayFx(bonus1_fx);
			}

			//Checking for activator-deactivator sensors
			p2List_item<activableBodies>* item = activable.getFirst();
			while (item && !found)
			{
				if (bodyB == item->data.deactivator)
				{
					item->data.Deactivate();
				}
				if (bodyB == item->data.activator)
				{
					item->data.Activate();
				}
				item = item->next;
			}

			//Checking for any light sensors pressed
			p2List_item<lightSwitch*>* currentLight = lights.getFirst();
			while (currentLight && !found)
			{
				if (bodyB == currentLight->data->sensor)
				{
					if (currentLight->data->counter > 100)
					{
						score += currentLight->data->scoreGiven;
						currentLight->data->lights_on++;
						currentLight->data->counter = 0;
						if (currentLight->data->extraBall && currentLight->data->lights.count()+1 == currentLight->data->lights_on)
						{
							//Resetting lights and adding an extra ball if necessary
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
		} // end of if (find.ball != -1)
	} // end of if( bodyB && ball)
}



//Keep minimized whenever possible, chaos down there!
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
		668, 648,
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
		521, 395,
		515, 395,
		536, 505,
		434, 552
	};
	App->physics->CreateChain(rightStick, 12);

	int leftStick[12] = {
		210, 556,
		108, 507,
		138, 395,
		143, 395,
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
		362, 50,
		369, 50,
		369, 44
	};
	App->physics->CreateChain(miniRight, 10);

	int miniLeft[10] = {
		332, 40,
		329, 42,
		329, 50,
		336, 50,
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
		307, 40,
		264, 46,
		251, 48,
		233, 54,
		220, 63,
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
	553, 190,
	525, 245
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
		270, 32,
		270, 9,
		295, 14,
		296, 32
	};

	int topLeftActivator[8] = {
		316, 38,
		330, 13,
		343, 14,
		323, 42
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
		430, 33,
		430, 7,
		404, 7,
		398, 34
	};

	int topRightActivator[8] = {
		379, 35,
		377, 8,
		370, 7,
		375, 35
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

	int miniLightLeftP[8] = {
		310, 55,
		310, 25,
		327, 25,
		327, 55
	};

	return ret;
}