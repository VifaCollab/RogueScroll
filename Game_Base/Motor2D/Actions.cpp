#include "Actions.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1EntitiesManager.h"
#include "j1Gui.h"
#include "j1Player.h"
#include "j1InputManager.h"
#include "j1Physics.h"

#include "UI_Element.h"
#include "UI_String.h"

/// Action --------------------------------------
// Constructors =======================
Action::Action(ACTION_TYPE action_type): action_type(action_type)
{

}

// Destructors ========================
Action::~Action()
{

}

// Game Loop ==========================
bool Action::Init()
{
	return true;
}

bool Action::Execute()
{
	return true;
}
/// ---------------------------------------------

/// Dialog_Action -------------------------------
// Constructors =======================
Dialog_Action::Dialog_Action(): Action(DIALOG_ACTION)
{

}

// Destructors ========================
Dialog_Action::~Dialog_Action()
{

}

// Game Loop ==========================
bool Dialog_Action::Init()
{
	LOG("DIALOG_");

	//Stop target body when dialog starts & block input
	target->GetBody()->body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
	target->LockInput();

	return true;
}

bool Dialog_Action::Execute()
{
	//Get the first dialog string
	if (current_dialog == nullptr)
	{
		current_dialog = ((NPC*)actor)->GetDialogByIndex(0);
		current_dialog->SetBoxPosition(DEFAULT_DIALOG_X - current_dialog->GetBox()->w, DEFAULT_DIALOG_Y - current_dialog->GetBox()->h);
	}

	//Focus the next dialog string
	else if (App->input_manager->GetEvent(INPUT_EVENT::INTERACT) == INPUT_DOWN)
	{
		//Get the next dialog phrase
		current_dialog = ((NPC*)actor)->GetDialogByIndex(++dialog_index);
		
		//When dialog ends player input is unlocked
		if (current_dialog == nullptr)
		{
			LOG("DIALOG_ENDED_");
			target->UnlockInput();
			return true;
		}

		//Set the correct dialog box position
		else current_dialog->SetBoxPosition(DEFAULT_DIALOG_X - current_dialog->GetBox()->w, DEFAULT_DIALOG_Y - current_dialog->GetBox()->h);
	}

	//Draw the current string
	current_dialog->Draw(false);

	return false;
}
/// ---------------------------------------------

/// Spawn_Delay_Action --------------------------
// Constructors =======================
Spawn_Delay_Action::Spawn_Delay_Action() : Action(SPAWN_DELAY_ACTION)
{

}

// Destructors ========================
Spawn_Delay_Action::~Spawn_Delay_Action()
{

}

// Game Loop ==========================
bool Spawn_Delay_Action::Init()
{
	if (actor == nullptr || actor->GetBody() == nullptr)return false;

	LOG("Spaw_delay!");
	//Get the previous fixture 
	prev_collision_type = actor->GetBody()->collision_type;
	
	//Change the current entity fixture to a ghost one
	b2Fixture* prev_fixture = actor->GetBody()->body->GetFixtureList();

	b2FixtureDef temp_fixture;
	temp_fixture.shape = prev_fixture->GetShape();
	temp_fixture.density = prev_fixture->GetDensity();
	temp_fixture.restitution = prev_fixture->GetRestitution();
	temp_fixture.friction = prev_fixture->GetFriction();
	temp_fixture.isSensor = false;
	temp_fixture.userData = prev_fixture->GetUserData();
	App->physics->SetFixture(temp_fixture, GHOST_COLLISION);
	actor->GetBody()->body->CreateFixture(&temp_fixture);
	actor->GetBody()->body->DestroyFixture(prev_fixture);

	delay_timer.Start();

	return true;
}

bool Spawn_Delay_Action::Execute()
{
	if (this->delay_timer.Read() > delay)
	{
		//Change the current ghost fixture the original
		b2Fixture* prev_fixture = actor->GetBody()->body->GetFixtureList();

		b2FixtureDef temp_fixture;
		temp_fixture.shape = prev_fixture->GetShape();
		temp_fixture.density = prev_fixture->GetDensity();
		temp_fixture.restitution = prev_fixture->GetRestitution();
		temp_fixture.friction = prev_fixture->GetFriction();
		temp_fixture.isSensor = false;
		temp_fixture.userData = prev_fixture->GetUserData();
		App->physics->SetFixture(temp_fixture, prev_collision_type);
		actor->GetBody()->body->CreateFixture(&temp_fixture);
		actor->GetBody()->body->DestroyFixture(prev_fixture);

		return true;
	}

	return false;
}
/// ---------------------------------------------

/// Move_Action ---------------------------------
// Constructors =======================
Move_Action::Move_Action(const iPoint & destination) : Action(MOVE_ACTION), destination(destination)
{

}

// Destructors ========================
Move_Action::~Move_Action()
{

}

// Game Loop ==========================
bool Move_Action::Init()
{
	//Get actor position
	int act_x = 0, act_y = 0;
	actor->GetBody()->GetPosition(act_x, act_y);

	//Check if the actor is already in the destination
	if (abs(act_x - destination.x) < LOCATION_ERROR_MARGIN)return false;

	//Set actor movement direction
	if (destination.x < act_x)
	{
		mov_direction = -1;
	}
	else
	{
		mov_direction = 1;
	}

	return true;
}

bool Move_Action::Execute()
{
	//Get actor position
	int act_x = 0, act_y = 0;
	actor->GetBody()->GetPosition(act_x, act_y);

	//Calculate the distance to the goal
	int dist_to_goal = act_x - destination.x;

	//Check if the actor has reached the goal
	while (abs(dist_to_goal) > LOCATION_ERROR_MARGIN)
	{
		actor->GetBody()->body->SetLinearVelocity(b2Vec2(((Creature*)actor)->GetMovSpeed() * mov_direction, 0));
		return false;
	}

	//Stop the actor body when is in the goal
	actor->GetBody()->body->SetLinearVelocity(b2Vec2(0, 0));

	return true;
}
/// ---------------------------------------------

/// Move_To_Target_Action -----------------------
// Constructors =======================
Move_To_Target_Action::Move_To_Target_Action()
{

}

// Destructors ========================
Move_To_Target_Action::~Move_To_Target_Action()
{

}

// Game Loop ==========================
bool Move_To_Target_Action::Init()
{
	//Calculate the goal
	int tar_x, tar_y;
	target->GetBody()->GetPosition(tar_x, tar_y);
	int act_x, act_y;
	actor->GetBody()->GetPosition(act_x, act_y);

	float mid_range = ((Intelligent_Creature*)actor)->GetVisionArea()->width * 0.5;
	
	//Check if the entity is inside goal area
	if (abs(tar_x - act_x) < mid_range)
	{
		return false;
	}
	
	if (tar_x < act_x)
	{
		current_goal.x = tar_x + mid_range;
		mov_direction = -1;
	}
	else
	{
		current_goal.x = tar_x - mid_range;
		mov_direction = 1;
	}

	//Initialize the update timer
	update_timer.Start();

	return true;
}

bool Move_To_Target_Action::Execute()
{
	//Get actor position
	int act_x = 0, act_y = 0;
	actor->GetBody()->GetPosition(act_x, act_y);

	//Update the goal
	if (update_timer.Read() > ACTIONS_UPDATE_RATE)
	{
		this->Init();
	}
	
	//Calculate the distance to the goal
	int dist_to_goal = act_x - current_goal.x;

	//Check if the actor has reached the goal
	while (abs(dist_to_goal) > LOCATION_ERROR_MARGIN)
	{
		actor->GetBody()->body->SetLinearVelocity(b2Vec2(((Creature*)actor)->GetMovSpeed() * mov_direction, 0));
		return false;
	}

	//Stop the actor body when is in the goal
	actor->GetBody()->body->SetLinearVelocity(b2Vec2(0, 0));

	return true;
}
/// ---------------------------------------------

/// Basic_Attack Action -------------------------
// Constructors =======================
Basic_Attack_Action::Basic_Attack_Action() : Action(BASIC_ATTACK_ACTION)
{

}

// Destructors ========================
Basic_Attack_Action::~Basic_Attack_Action()
{

}

// Game Loop ==========================
bool Basic_Attack_Action::Execute()
{

	LOG("ATTACK!");
	return true;
}
/// ---------------------------------------------


