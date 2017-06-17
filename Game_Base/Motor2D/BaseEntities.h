#include "p2Point.h"
#include "p2Defs.h"
#include <string>

using namespace std;
class PhysBody;
class Animation;

enum ENTITY_TYPE
{
	NO_ENTITY = 0,
	CREATURE,
	ITEM
};

enum CREATURE_TYPE
{
	NO_CREATURE = 0,
	PLAYER_CREATURE
};

enum ITEM_TYPE
{
	NO_ITEM = 0
};

/// Entity --------------------------------------
//Base class where the entity pillars are defined
class Entity
{
public:

	Entity();
	Entity(const Entity& copy);
	virtual ~Entity();

protected:

	ENTITY_TYPE	entity_type = NO_ENTITY;
	PhysBody*	body = nullptr;
	string		name = "";
	string		description = "";
	//Animation*	current_animation = nullptr;

public:

	//Set Methods -----------
	void SetEntityType(ENTITY_TYPE type);
	void SetName(string new_name);
	void SetDescription(string new_description);
	void SetBody(PhysBody* new_body);

	//Get Methods -----------
	ENTITY_TYPE	GetEntityType()const;
	const char*	GetName()const;
	const char*	GetDescription()const;
	PhysBody*	GetBody()const;

};
/// ---------------------------------------------

/// Creature ------------------------------------
class Creature : public Entity
{
public:

	Creature();
	Creature(const Creature& copy);
	~Creature();

private:

	CREATURE_TYPE creature_type = NO_CREATURE;

	uint life = 0;
	uint attack = 0;
	float mov_speed = 0.0f;
	float jump_force = 0.0f;

public:

	//Set Methods -----------
	void SetCreatureType(CREATURE_TYPE new_creature_type);
	void SetLife(uint new_life);
	void SetAttack(uint new_attack);
	void SetMovSpeed(float new_mov_speed);
	void SetJumpForce(float new_jump_force);

	//Get Methods -----------
	CREATURE_TYPE	GetCreatureType()const;
	uint			GetLife()const;
	uint			GetAttack()const;
	float			GetMovSpeed()const;
	float			GetJumpForce()const;

};
/// ---------------------------------------------