#include "j1Player.h"

#include "j1App.h"
#include "j1EntitiesManager.h"
#include "j1InputManager.h"
#include "j1Physics.h"
#include "j1Render.h"
#include "j1Scene.h"

// Constructors =================================
j1Player::j1Player()
{
	name = "player";
}

// Destructors ==================================
j1Player::~j1Player()
{
	
}

// Game Loop ====================================
bool j1Player::Start()
{
	avatar = App->entities_manager->GenerateCreature(PLAYER_CREATURE);
	avatar->GetBody()->SetPosition(1500, 900);
	avatar->SetMovSpeed(2500.0f);
	avatar->GetBody()->body->GetFixtureList()->SetFriction(0.0f);

	return true;
}

bool j1Player::Update(float dt)
{
	// Read all player inputs states
	INPUT_STATE go_left_input_state = App->input_manager->GetEvent(INPUT_EVENT::GO_LEFT);
	INPUT_STATE go_right_input_state = App->input_manager->GetEvent(INPUT_EVENT::GO_RIGHT);
	INPUT_STATE jump_input_state = App->input_manager->GetEvent(INPUT_EVENT::JUMP);
	INPUT_STATE crouch_input_state = App->input_manager->GetEvent(INPUT_EVENT::CROUCH);
	
	
	// Get player necessary data
	int x_pos = 0, y_pos = 0;
	avatar->GetBody()->GetPosition(x_pos, y_pos);
	b2Vec2 linear_vel = avatar->GetBody()->body->GetLinearVelocity();


	// Center camera at player position ---------
	App->render->camera.x = -x_pos + PLAYER_CAMERA_X;
	App->render->camera.y = -y_pos + PLAYER_CAMERA_Y;

	// JUMP INPUT -------------------------------
	if (jump_input_state == INPUT_DOWN)
	{
		avatar->GetBody()->body->ApplyForceToCenter(b2Vec2(0.0f, -30.0f), true);
	}

	// AIR INPUT --------------------------------
	if (!avatar->GetBody()->IsInContact())
	{
		if (linear_vel.x < 0)
		{
			App->scene->UpdateParallax(-avatar->GetMovSpeed() * App->GetDT());
		}
		else if (linear_vel.x > 0)
		{
			App->scene->UpdateParallax(avatar->GetMovSpeed() * App->GetDT());
		}
		return true;
	}

	// ARROWS INPUT -----------------------------
	if (go_left_input_state == INPUT_REPEAT)
	{
		avatar->GetBody()->body->SetLinearVelocity(b2Vec2(-avatar->GetMovSpeed() * App->GetDT(), linear_vel.y));
		App->scene->UpdateParallax(-avatar->GetMovSpeed() * App->GetDT());
	}
	else if (go_right_input_state == INPUT_REPEAT)
	{
		avatar->GetBody()->body->SetLinearVelocity(b2Vec2(avatar->GetMovSpeed() * App->GetDT(), linear_vel.y));
		App->scene->UpdateParallax(avatar->GetMovSpeed() * App->GetDT());
	}
	else
	{
		avatar->GetBody()->body->SetLinearVelocity(b2Vec2(0.0f, linear_vel.y));
	}





	return true;
}
