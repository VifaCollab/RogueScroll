#include "Worker.h"
#include <stdarg.h>
#include "p2Log.h"

/// Worker --------------------------------------
// Constructors =======================
Worker::Worker()
{

}

// Destructors ========================
Worker::~Worker()
{
	std::list<Action*>::const_iterator act = actions.begin();
	while (act != actions.end())
	{
		RELEASE(act._Ptr->_Myval);
		act++;
	}
	actions.clear();

	if (current_action != nullptr)RELEASE(current_action);
}

// Game Loop ==========================
void Worker::Update()
{
	// Execute the current action
	if (current_action != nullptr)
	{
		if (current_action->Execute()) RELEASE(current_action);
	}

	// Start a new action
	else if (!actions.empty())
	{
		current_action = actions.front();
		actions.pop_front();

		// Check if the new action is started correctly 
		if (current_action != nullptr && !current_action->Init())
		{
			RELEASE(current_action);
		}
	}
}

Action * Worker::GetCurrentAction() const
{
	return current_action;
}

LG_ACTION_TYPE Worker::GetCurrentActionType() const
{
	if (current_action == nullptr)return LG_NO_ACTION;
	return current_action->action_type;
}

// Action Factory =====================
Action * Worker::GenerateAction(LG_ACTION_TYPE type, ...)
{
	//Build list of variables
	va_list variables;
	va_start(variables, type);

	//Empty action pointer
	Action* new_act = nullptr;

	//Allocate specific class depending of the action type
	switch (type)
	{
	case LG_DIALOG_ACTION:
		//Generate dialog action
		new_act = new Dialog_Action();
		//Set action stats
		new_act->actor = va_arg(variables, Entity*);
		((Dialog_Action*)new_act)->target = va_arg(variables, Player*);
		break;

	case LG_SPAWN_DELAY_ACTION:
		//Generate spawn delay action
		new_act = new Spawn_Delay_Action();
		//Set action stats
		new_act->actor = va_arg(variables, Entity*);
		((Spawn_Delay_Action*)new_act)->delay = va_arg(variables, int);
		break;

	case LG_MOVE_ACTION:
		//Generate move action
		new_act = new Move_Action();
		//Set action stats
		new_act->actor = va_arg(variables, Entity*);
		((Move_Action*)new_act)->destination = va_arg(variables, iPoint);
		break;

	case LG_MOVE_TO_TARGET_ACTION:
		//Generate move to target action
		new_act = new Move_To_Target_Action();
		//Set action stats
		new_act->actor = va_arg(variables, Entity*);
		((Move_To_Target_Action*)new_act)->target = va_arg(variables, Creature*);
		break;

	case LG_BASIC_ATTACK_ACTION:
		//Generate basic attack action
		new_act = new Basic_Attack_Action();
		//Set action stats
		new_act->actor = va_arg(variables, Entity*);
		((Basic_Attack_Action*)new_act)->target = va_arg(variables, Creature*);
		break;

	case LG_SIMPLE_ATTACK_ACTION:
		//Generate simple attack action
		new_act = new Simple_Attack_Action();
		//Set action stats
		new_act->actor = va_arg(variables, Entity*);
		((Simple_Attack_Action*)new_act)->target = va_arg(variables, Creature*);
		break;

	case LG_DIE_ACTION:
		//Generate die action
		new_act = new Die_Action();
		//Set action stats
		new_act->actor = va_arg(variables, Entity*);
		break;

	case LG_STUN_ACTION:
		//Generate die action
		new_act = new Stun_Action();
		//Set action stats
		new_act->actor = va_arg(variables, Entity*);
		((Stun_Action*)new_act)->time = va_arg(variables, int);
		((Stun_Action*)new_act)->blit_color = va_arg(variables, SDL_Color);
		break;
		
	case LG_MAGIC_SPAWN_ACTION:
		//Generate die action
		new_act = new Magic_Spawn_Action();
		//Set action stats
		new_act->actor = va_arg(variables, Entity*);
		((Magic_Spawn_Action*)new_act)->total_time = va_arg(variables, int);
		((Magic_Spawn_Action*)new_act)->flips = va_arg(variables, int);
		((Magic_Spawn_Action*)new_act)->initial_scale = va_arg(variables, double);
		break;
	}

	//Return the generated action
	if (new_act != nullptr)return new_act;
	else LOG("Error generating logical action!");

	return nullptr;
}

// Actions Management =================
void Worker::AddAction(const Action * new_act)
{
	actions.push_back((Action*)new_act);
}

void Worker::AddPriorizedAction(const Action * new_act)
{
	if (current_action != nullptr)
	{
		actions.push_front(current_action);
	}
	current_action = (Action*)new_act;
	current_action->Init();
}
/// ---------------------------------------------
