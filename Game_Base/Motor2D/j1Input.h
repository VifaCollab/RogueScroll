#ifndef __j1INPUT_H__
#define __j1INPUT_H__

#include "j1Module.h"
#include "SDL\include\SDL_gamecontroller.h"

//#define NUM_KEYS 352
#define NUM_MOUSE_BUTTONS 5
#define NUM_CONTROLLER_BUTTONS 15
#define NUM_CONTROLLER_AXIS 6
#define DEAD_ZONE 10000
#define TRIGGER_ZONE 32000
//#define LAST_KEYS_PRESSED_BUFFER 50

struct SDL_Rect;

enum j1EventWindow
{
	WE_QUIT = 0,
	WE_HIDE = 1,
	WE_SHOW = 2,
	WE_COUNT
};

enum j1KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

enum j1JoystickState
{
	JOYSTICK_POSITIVE,
	JOYSTICK_NEGATIVE,
	JOYSTICK_NOTHING
};

class j1Input : public j1Module
{

public:

	j1Input();

	// Destructor
	virtual ~j1Input();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();

	// Called before quitting
	bool CleanUp();

	// Gather relevant win events
	bool GetWindowEvent(j1EventWindow ev);

	// Check key states (includes mouse and joy buttons)
	j1KeyState GetKey(int id) const
	{
		return keyboard[id];
	}

	j1KeyState GetMouseButtonDown(int id) const
	{
		return mouse_buttons[id - 1];
	}

	// Get mouse / axis position
	void GetMousePosition(int &x, int &y);
	void GetMouseMotion(int& x, int& y);

	//	Get mouse x/y coordinates
	int GetMouseX() const;
	int GetMouseY() const;

	// Reset all input states
	void ResetInputMaps();

private:

	bool		windowEvents[WE_COUNT];
	j1KeyState*	keyboard = nullptr;
	
	// Mouse data -----------
	j1KeyState	mouse_buttons[NUM_MOUSE_BUTTONS];
	int			mouse_motion_x = 0;
	int			mouse_motion_y = 0;
	int			mouse_x = 0;
	int			mouse_y = 0;

	// Controller data ------
	SDL_GameController*	gamecontroller = nullptr;
	j1KeyState			controller_buttons[NUM_CONTROLLER_BUTTONS];
	j1JoystickState		controller_axis[NUM_CONTROLLER_AXIS];
};

#endif // __j1INPUT_H__