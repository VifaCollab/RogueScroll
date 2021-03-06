#ifndef _J1_INPUT_MANAGER_H_
#define _J1_INPUT_MANAGER_H_

#include "j1Module.h"
#include <map>

#include "j1App.h"

enum INPUT_EVENT
{
	UNKNOWN_INPUT = 0,
	
	//Escape
	ESCAPE,

	//Debug
	COLLIDERS_DEBUG_MODE,
	UI_DEBUG_MODE,

	//Player
	GO_LEFT,
	GO_RIGHT,
	JUMP,
	CROUCH,
	INTERACT,
	ATTACK,

	//Menu/UI
	FOCUS_NEXT,
	FOCUS_PREV,
	ACCEPT,
	ADD_VALUE,
	REST_VALUE
};

enum INPUT_STATE
{
	INPUT_NONE = 0,
	INPUT_UP,
	INPUT_DOWN,
	INPUT_REPEAT,
	JSTICK_NONE,
	JSTICK_POSITIVE,
	JSTICK_NEGATIVE
};

/// Suitable_Input_Event --------------
struct Suitable_Input_Event
{
	//Constructor/Destructor
	Suitable_Input_Event();
	Suitable_Input_Event(INPUT_EVENT input_event, APP_CONTEXT app_context);
	Suitable_Input_Event(const Suitable_Input_Event& copy);
	~Suitable_Input_Event();

	//Operator used for inserting events in the multimap
	bool operator < (const Suitable_Input_Event& target)const;

	//Data
	INPUT_EVENT	input_event = UNKNOWN_INPUT;
	APP_CONTEXT	app_context = UNKNOWN_CONTEXT;

};
/// -----------------------------------

class j1InputManager : public j1Module
{
public:

	j1InputManager();
	~j1InputManager();

public:

	// Called when before render is available
	bool Awake(pugi::xml_node& );

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

public:
	
	//Functionality -------------------
	//Used when loading input keys
	INPUT_EVENT				StrToInputEvent(const char* str)const;
	std::pair<int, int>		StrToControllerJoyID(const char* str)const;
	//Used to manage the programmed input events
	void					SendKeyboardInputEvent(int id, INPUT_STATE state);
	void					SendControllerInputEvent(int id, INPUT_STATE state);
	void					SendControllerJoystickEvent(int id, INPUT_STATE state);
	INPUT_STATE				GetEvent(INPUT_EVENT _event);

private:

	//Maps with all the input events mapped
	std::multimap<int, Suitable_Input_Event>				keyboard_events_map; /*for the keyboard*/
	std::multimap<int, Suitable_Input_Event>				controller_events_map; /*for the controller*/
	std::multimap<std::pair<int,int>, Suitable_Input_Event> controller_joysticks_events_map; /*for the controller axis (track sdl & my ID)*/

	//Events that are happening during this frame
	std::multimap<Suitable_Input_Event, INPUT_STATE> current_events;
};
#endif // _J1_INPUT_MANAGER_H_
