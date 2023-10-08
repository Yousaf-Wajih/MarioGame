#include "Mario.h"
#include "Object.h"
#include "Resources.h"

#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_fixture.h>
#include <iostream>
#include "Game.h"
#include "Enemy.h"

const float movementSpeed = 7.0f;
const float jumpVelocity = 10.0f;

void Mario::Begin()
{
	runAnimation = Animation(0.45f,
		{
			AnimFrame(0.30f, Resources::textures["run3.png"]),
			AnimFrame(0.15f, Resources::textures["run2.png"]),
			AnimFrame(0.00f, Resources::textures["run1.png"]),
		});

	jumpSound.setBuffer(Resources::sounds["jump.wav"]);
	jumpSound.setVolume(20);

	fixtureData.listener = this;
	fixtureData.mario = this;
	fixtureData.type = FixtureDataType::Mario;

	b2BodyDef bodyDef{};
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(position.x, position.y);
	bodyDef.fixedRotation = true;
	body = Physics::world->CreateBody(&bodyDef);

	b2FixtureDef fixtureDef{};
	fixtureDef.userData.pointer = (uintptr_t)&fixtureData;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.0f;

	b2CircleShape circleShape{};
	circleShape.m_radius = 0.5f;
	circleShape.m_p.Set(0.0f, -0.5f);
	fixtureDef.shape = &circleShape;
	body->CreateFixture(&fixtureDef);

	circleShape.m_p.Set(0.0f, 0.5f);
	body->CreateFixture(&fixtureDef);

	b2PolygonShape polygonShape{};
	polygonShape.SetAsBox(0.5f, 0.5f);
	fixtureDef.shape = &polygonShape;
	body->CreateFixture(&fixtureDef);

	polygonShape.SetAsBox(0.4f, 0.2f, b2Vec2(0.0f, 1.0f), 0.0f);
	fixtureDef.isSensor = true;
	groundFixture = body->CreateFixture(&fixtureDef);
}

void Mario::Update(float deltaTime)
{
	float move = movementSpeed;

	runAnimation.Update(deltaTime);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
		move *= 2;

	b2Vec2 velocity = body->GetLinearVelocity();
	velocity.x = 0.0f;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		velocity.x += move;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		velocity.x -= move;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && isGrounded)
	{
		velocity.y = -jumpVelocity;
		jumpSound.play();
	}

	textureToDraw = runAnimation.GetTexture();

	if (velocity.x < -0.02f)
		facingLeft = true;
	else if (velocity.x > 0.02f)
		facingLeft = false;
	else
		textureToDraw = Resources::textures["idle.png"];

	if (!isGrounded)
		textureToDraw = Resources::textures["jump.png"];

	body->SetLinearVelocity(velocity);

	position = sf::Vector2f(body->GetPosition().x, body->GetPosition().y);
	angle = body->GetAngle() * (180.0f / M_PI);
}

void Mario::Draw(Renderer& renderer)
{
	renderer.Draw(textureToDraw, position,
		sf::Vector2f(facingLeft ? -1.0f : 1.0f, 2.0f), angle);
}

void Mario::OnBeginContact(b2Fixture* self, b2Fixture* other)
{
	FixtureData* data = (FixtureData*)other->GetUserData().pointer;

	if (!data)
		return;

	if (groundFixture == self && data->type == FixtureDataType::MapTile)
		isGrounded++;
	else if (data->type == FixtureDataType::Object && data->object->tag == "coin")
	{
		DeleteObject(data->object);
		std::cout << "coins = " << ++coins << "\n";
	}
	else if (data->type == FixtureDataType::Object && data->object->tag == "enemy")
	{
		Enemy* enemy = dynamic_cast<Enemy*>(data->object);
		if (!enemy)
			return;

		if (groundFixture == self)
		{
			enemy->Die();
		}
		else if (!enemy->IsDead())
		{
			isDead = true;
		}
	}
}

void Mario::OnEndContact(b2Fixture* self, b2Fixture* other)
{
	FixtureData* data = (FixtureData*)other->GetUserData().pointer;

	if (!data)
		return;

	if (groundFixture == self &&
		data->type == FixtureDataType::MapTile && isGrounded > 0)
		isGrounded--;
}

size_t Mario::GetCoins()
{
	return coins;
}
