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

// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody() : body(NULL), listener(NULL), dead(false), joint(NULL)
	{}

	~PhysBody(){
		body->GetWorld()->DestroyBody(body);
	};

	void GetPosition(int& x, int &y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;

public:
	int width, height;
	b2Body* body;
	Module* listener;
	b2RevoluteJoint* joint;
	bool dead;
	// TODO 6: Add a pointer to a module that might want to listen to a collision from this body
};

// Module --------------------------------------
// TODO 3: Make module physics inherit from b2ContactListener
// then override void BeginContact(b2Contact* contact)
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
	PhysBody* CreateFlipper(int* points, int pivotX, int pivotY, int right);
	PhysBody* CreateLauncher(int* points, int size, int pivotX, int pivotY);

	void BeginContact(b2Contact* contact);

	void InvertGravity();



	bool debug;
	b2World* world;

private:

};