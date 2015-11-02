#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "p2Point.h"
#include "math.h"

#ifdef _DEBUG
#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
#endif

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled), b2ContactListener()
{
	world = NULL;
	debug = true;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}


bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	world = new b2World(b2Vec2(GRAVITY_X, GRAVITY_Y));
	world->SetContactListener((b2ContactListener*)this);

	return true;
}

void ModulePhysics::Bounce(PhysBody* movable, PhysBody* non_movable, float intensity)
{
	b2Vec2 force;
	b2Vec2 movableCenter;
	b2Vec2 non_movableCenter;

	int x, y;
	movable->GetPosition(x, y);

	movableCenter.x = x;
	movableCenter.y = y;

	non_movable->GetPosition(x, y);
	non_movableCenter.x = x;
	non_movableCenter.y =y;

	movable->RayCast(movableCenter.x, movableCenter.y, non_movableCenter.x, non_movableCenter.y, force.x, force.y);
	force.Normalize();
	force *= intensity;
	movable->body->ApplyForceToCenter(force, true);

	//DEBUG
	pos1 = movableCenter;
	pos2 = non_movableCenter;
	norm = force;
	//////

}

// 
update_status ModulePhysics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 6, 2);

	return UPDATE_CONTINUE;
}

PhysBody* ModulePhysics::CreateBall(int x, int y)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	PhysBody* pbody = new PhysBody();
	body.userData = pbody;

	b2Body* b = world->CreateBody(&body);

	b->SetBullet(true);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(BALL_RADIUS);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 0.5f;

	b->CreateFixture(&fixture);

	pbody->body = b;
	pbody->width = pbody->height = BALL_RADIUS;

	return pbody;
}

PhysBody* ModulePhysics::CreateChain(int* points, int size)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(0,0);

	PhysBody* pbody = new PhysBody();
	body.userData = pbody;

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	int averageX = 0;
	int averageY = 0;
	uint i = 0;
	for (; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
		averageX += points[i * 2 + 0];
		averageY += points[i * 2 + 1];
	}
	pbody->SetPos(averageX / i, averageY / i);

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.density = 1.0f;
	fixture.shape = &shape;

	b->CreateFixture(&fixture);

	delete p;


	pbody->body = b;
	pbody->width = pbody->height = 0;

	return pbody;
}

PhysBody* ModulePhysics::CreatePolygon(int* points, int size)
{
	if (size > 8)
	{
		return NULL;
	}
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(0, 0);

	PhysBody* pbody = new PhysBody();
	body.userData = pbody;

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	int averageX = 0;
	int averageY = 0;
	uint i = 0;
	for (; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
		averageX += points[i * 2 + 0];
		averageY += points[i * 2 + 1];
	}
	pbody->SetPos(averageX / i, averageY / i);

	shape.Set(p, size / 2);

	b2FixtureDef fixture;
	fixture.density = 1.0f;
	fixture.shape = &shape;

	b->CreateFixture(&fixture);

	delete p;


	pbody->body = b;
	pbody->width = pbody->height = 0;

	return pbody;
}

PhysBody* ModulePhysics::CreateChain(int* points, int size, float restitution)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(0,0);

	PhysBody* pbody = new PhysBody();
	body.userData = pbody;

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	int averageX = 0;
	int averageY = 0;
	uint i = 0;
	for (; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
		averageX += points[i * 2 + 0];
		averageY += points[i * 2 + 1];
	}
	pbody->SetPos(averageX / i, averageY / i);

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;
	fixture.restitution = restitution;
	b->CreateFixture(&fixture);

	delete p;


	pbody->body = b;
	pbody->width = pbody->height = 0;

	return pbody;
}

PhysBody* ModulePhysics::CreateSensor(int* points, int size)
{
	PhysBody* ret = CreateChain(points, size);
	ret->body->GetFixtureList()->SetSensor(true);
	return ret;
}

PhysBody* ModulePhysics::CreateFlipper(int* points, int pivotX, int pivotY, int right)
{

	b2BodyDef body;
	body.type = b2_dynamicBody;

	body.position.Set(PIXEL_TO_METERS(pivotX), PIXEL_TO_METERS(pivotY));
	
	PhysBody* pbody = new PhysBody();
	body.userData = pbody;

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape shape;

	b2Vec2* p = new b2Vec2[8]; //  16/2

	for (uint i = 0; i < 8; ++i)
	{
		int point = points[i * 2 + 0] - pivotX;
		p[i].x = PIXEL_TO_METERS(point);
		point = points[i * 2 + 1] - pivotY;
		p[i].y = PIXEL_TO_METERS(point);
	}

	shape.Set(p, 8);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	pbody->body = b;
	pbody->width = pbody->height = 0;

	int pivPos[8] = { pivotX - 2, pivotY - 2, pivotX + 2, pivotY - 2, pivotX + 2, pivotY + 2, pivotX - 2, pivotY + 2 };
	PhysBody* pivot = CreateChain(pivPos, 8);
	
	b2RevoluteJointDef joint;
	joint.Initialize(pivot->body, b, b2Vec2(PIXEL_TO_METERS(pivotX), PIXEL_TO_METERS(pivotY)));
	joint.collideConnected = false;
	if (right)
	{
		joint.lowerAngle = 0.0f * DEGTORAD;
		joint.upperAngle = 60.0f * DEGTORAD;;
	}
	else
	{
		joint.lowerAngle = -60.0f * DEGTORAD;
		joint.upperAngle = 0.0f * DEGTORAD;
	}
	joint.enableLimit = true;
	joint.maxMotorTorque = 5000.0f;
	joint.type = e_revoluteJoint;
	pbody->joint = (b2RevoluteJoint*)world->CreateJoint(&joint);
	
	delete p;

	return pbody;
}

PhysBody* ModulePhysics::CreateLauncher(int* points, int size, int pivotX, int pivotY)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(0, 0);

	PhysBody* pbody = new PhysBody();
	body.userData = pbody;

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	int averageX = 0;
	int averageY = 0;
	uint i = 0;
	for (; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
		averageX += points[i * 2 + 0];
		averageY += points[i * 2 + 1];
	}
	pbody->SetPos(averageX / i, averageY / i);

	shape.CreateLoop(p, size / 2);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;
	b->SetGravityScale(10);
	b->CreateFixture(&fixture);
	delete p;
	pbody->body = b;
	pbody->width = pbody->height = 0;

	int pivPos[8] = { pivotX - 2, pivotY - 2, pivotX + 2, pivotY - 2, pivotX + 2, pivotY + 2, pivotX - 2, pivotY + 2 };
	PhysBody* pivot = CreateChain(pivPos, 8);

	b2PrismaticJointDef joint;
	joint.Initialize(b, pivot->body, b2Vec2(pivotX, pivotY), b2Vec2(0.19f, 0.75f));

	joint.motorSpeed = 15.0f;
	joint.maxMotorForce = 1000000.0f;
	joint.enableMotor = false;
	
	joint.upperTranslation = 8.0f;
	joint.lowerTranslation = 0.0f;
	joint.enableLimit = true;
	

	joint.type = e_prismaticJoint;
	world->CreateJoint(&joint);


	return pbody;
}

// 
update_status ModulePhysics::PostUpdate()
{
	
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if (!debug)
		return UPDATE_CONTINUE;

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{

		//DEBUG (raycast)
		App->renderer->DrawLine(pos1.x, pos1.y, pos2.x, pos2.y, 0, 255, 255);
		App->renderer->DrawLine(pos2.x + (norm.x), pos2.y + (norm.y), pos2.x, pos2.y, 255, 255, 0);

		for (b2JointEdge* j = b->GetJointList(); j; j = j->next)
		{
			App->renderer->DrawLine(METERS_TO_PIXELS(j->joint->GetAnchorA().x + 5), METERS_TO_PIXELS(j->joint->GetAnchorA().y + 5), METERS_TO_PIXELS(j->joint->GetAnchorA().x - 5), METERS_TO_PIXELS(j->joint->GetAnchorA().y - 5), 255, 0, 0);
			App->renderer->DrawLine(METERS_TO_PIXELS(j->joint->GetAnchorA().x - 5), METERS_TO_PIXELS(j->joint->GetAnchorA().y + 5), METERS_TO_PIXELS(j->joint->GetAnchorA().x + 5), METERS_TO_PIXELS(j->joint->GetAnchorA().y - 5), 255, 0, 0);
			App->renderer->DrawLine(METERS_TO_PIXELS(j->joint->GetAnchorA().x), METERS_TO_PIXELS(j->joint->GetAnchorA().y), METERS_TO_PIXELS(j->joint->GetAnchorB().x), METERS_TO_PIXELS(j->joint->GetAnchorB().y), 6, 255, 0, 0);
		}
		for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch (f->GetType())
			{
				// Draw circles ------------------------------------------------
			case b2Shape::e_circle:
			{
				b2CircleShape* shape = (b2CircleShape*)f->GetShape();
				b2Vec2 pos = f->GetBody()->GetPosition();
				App->renderer->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
			}
			break;

			// Draw polygons ------------------------------------------------
			case b2Shape::e_polygon:
			{
				b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
				int32 count = polygonShape->GetVertexCount();
				b2Vec2 prev, v;

				for (int32 i = 0; i < count; ++i)
				{
					v = b->GetWorldPoint(polygonShape->GetVertex(i));
					if (i > 0)
						App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);

					prev = v;
				}

				v = b->GetWorldPoint(polygonShape->GetVertex(0));
				App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
			}
			break;

			// Draw chains contour -------------------------------------------
			case b2Shape::e_chain:
			{
				b2ChainShape* shape = (b2ChainShape*)f->GetShape();
				b2Vec2 prev, v;

				for (int32 i = 0; i < shape->m_count; ++i)
				{
					v = b->GetWorldPoint(shape->m_vertices[i]);
					if (i > 0)
						App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
					prev = v;
				}

				v = b->GetWorldPoint(shape->m_vertices[0]);
				App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
			}
			break;

			// Draw a single segment(edge) ----------------------------------
			case b2Shape::e_edge:
			{
				b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
				b2Vec2 v1, v2;

				v1 = b->GetWorldPoint(shape->m_vertex0);
				v1 = b->GetWorldPoint(shape->m_vertex1);
				App->renderer->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
			}
			break;
			}
		}
	}

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	delete world;

	return true;
}

void PhysBody::GetPosition(int& x, int &y) const
{
	b2Vec2 position = body->GetPosition();
	if (position.x != 0 && position.y != 0)
	{
		x = METERS_TO_PIXELS(position.x) - (width);
		y = METERS_TO_PIXELS(position.y) - (height);
	}
	else
	{
		x = pos.x; y = pos.y;
	}
	
}

float PhysBody::GetRotation() const
{
	return RADTODEG * body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
	b2Fixture* fixture = body->GetFixtureList();
	do
	{
		if (fixture->GetShape()->TestPoint(body->GetTransform(), b2Vec2(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y))))
		{
			return true;
		}
	} while (fixture = fixture->GetNext());

	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;
	b2Fixture* fixture = body->GetFixtureList();
	b2RayCastOutput* rayOutput = new b2RayCastOutput;
	b2RayCastInput rayInput;
	rayInput.p1 = b2Vec2(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	rayInput.p2 = b2Vec2(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	rayInput.maxFraction = 1.0f;
	do {
		if (fixture->GetShape()->RayCast(rayOutput, rayInput, body->GetTransform(), 0))
		{
			normal_x = rayOutput->normal.x;
			normal_y = rayOutput->normal.y;
			b2Vec2 vect(x2 - x1, y2 - y1);
			ret = rayOutput->fraction * vect.Length();
		}
	} while (fixture->GetNext());
	
	return ret;
}

void PhysBody::SetPos(int x, int y)
{
	pos.x = x;
	pos.y = y;
}

bool PhysBody::Resize(float _scale)
{
	
	scale = _scale;
	body->DestroyFixture(body->GetFixtureList());

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(BALL_RADIUS * scale);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 0.5f;
	body->CreateFixture(&fixture);

	return true;
}

void ModulePhysics::BeginContact(b2Contact* contact)
{
	PhysBody* a = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysBody* b = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();

	if (a->listener)
	{
		a->listener->OnCollision(a, b);
	}
	if (b->listener)
	{
		b->listener->OnCollision(b, a);
	}
}

void ModulePhysics::InvertGravity()
{
	b2Vec2 gravity = world->GetGravity();
	gravity.y = gravity.y * -1;
	world->SetGravity(gravity);

}
