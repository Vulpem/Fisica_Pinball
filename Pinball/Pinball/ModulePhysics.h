#pragma once
#include "Module.h"
#include "Globals.h"
#include "Box2D/Box2D/Box2D.h"


#define GRAVITY_X 0.0f
#define GRAVITY_Y 6.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

#define BALL_RADIUS 8

// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody() : body(NULL), listener(NULL), dead(false), joint(NULL), scale(1.0f), pos(0.0f, 0.0f)
	{}

	~PhysBody();

	void GetPosition(int& x, int &y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;
	bool Resize(float _scale);
	void SetPos(int x, int y);

public:
	int width, height;
	b2Body* body;
	Module* listener;
	b2RevoluteJoint* joint;
	float scale;
	bool dead;

private:
	b2Vec2 pos;

};

struct activableBodies
{
	PhysBody* wall;
	PhysBody* activator;
	PhysBody* deactivator;

	bool shouldBeActive;

	activableBodies(PhysBody* _wall, PhysBody* _activator, PhysBody* _deactivator)
	{
		wall = _wall; activator = _activator; deactivator = _deactivator;
	}
	activableBodies(activableBodies& _source)
	{
		wall = _source.wall;
		activator = _source.activator;
		deactivator = _source.deactivator;
	}
	activableBodies()
	{
		wall = activator = deactivator = NULL;
	}

	void Activate()
	{
		shouldBeActive = true;
	}
	void Deactivate()
	{
		shouldBeActive = false;
	}
};

// Module --------------------------------------
class ModulePhysics : public Module, public b2ContactListener
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	PhysBody* CreateBall(int x, int y);
	PhysBody* CreateChain(int* points, int size);
	PhysBody* CreateChain(int* points, int size, float restitution);
	PhysBody* CreatePolygon(int* points, int size);
	PhysBody* CreateSensor(int* points, int size);
	PhysBody* CreateFlipper(int* points, int pivotX, int pivotY, int right);
	PhysBody* CreateLauncher(int* points, int size, int pivotX, int pivotY);

	void BeginContact(b2Contact* contact);

	void InvertGravity();
	void Bounce(PhysBody* movable, PhysBody* non_movable, float intensity);
	void Bounce(PhysBody* movable, b2Vec2 force);

	bool debug;
	b2World* world;

private:
	//DEBUG
	b2Vec2 pos1;
	b2Vec2 pos2;
	b2Vec2 norm;

};