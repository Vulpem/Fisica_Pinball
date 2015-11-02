
#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "Box2D/Box2D/Box2D.h"


PhysBody::~PhysBody()
{
	body->GetWorld()->DestroyBody(body);
	body = NULL;
	listener = NULL;
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