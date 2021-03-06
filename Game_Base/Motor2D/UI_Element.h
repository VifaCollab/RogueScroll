#ifndef _UI_ELEMENT_
#define _UI_ELEMENT_

#include "p2Point.h"
#include "p2Defs.h"
#include "j1App.h"
#include "j1Gui.h"
#include "SDL\include\SDL_rect.h"

enum UI_TYPE {

	UNDEFINED,
	BUTTON,
	TEXT_BOX,
	STRING,
	IMG,
	SCROLL,
	SCROLL_BAR,
	PROGRESSIVE_BAR,
	POPUP_MENU,
	FIXED_BUTTON
};

class UI_Element {
public:

	UI_Element(const SDL_Rect& box, UI_TYPE ui_type = UNDEFINED, bool IsActive = true);
	UI_Element(const UI_Element* copy);
	UI_Element();

	virtual ~UI_Element();

protected:

	SDL_Rect				box = { 0,0,0,0 };
	bool					use_camera = true;
	uint					visual_layer = 0;
	uint					logical_layer = 0;
	mutable bool			IsActive = true;
	mutable bool			blocked = false;
	bool					isin = false;
	UI_TYPE					ui_type = UNDEFINED;

	UI_Element*				parent = nullptr;

	j1Module*				input_target = nullptr;

public:

	std::list<UI_Element*>		childs;
	UI_Element*					next_in_focus = nullptr;
	UI_Element*					prev_in_focus = nullptr;

public:

	//Game Loop
	virtual bool	Update();
	bool			UpdateChilds();
	virtual bool	CleanUp();
	virtual void	Draw(bool debug) const;
	virtual void	DrawAt(int x, int y)const;
	void			DrawChilds(bool debug)const;
	virtual void	HandleInput();

	//Functionality
	//Box functions
	bool				MouseIsIn(int x = 0, int y = 0)const;
	void				SetBoxPosition(int new_pos_x, int new_pos_y);
	virtual void		MoveBox(int x_vel, int y_vel);
	void				ResizeBox(const iPoint& new_size);
	void				SetBox(SDL_Rect new_box);
	virtual SDL_Rect	AdjustBox();
	SDL_Rect*			GetBox()const;
	bool				RectIsIn(const SDL_Rect* target, int x_vel, int y_vel, bool x_axis = false)const;
	void				SetUseCamera(bool val);

	//Layer functions
	virtual void		SetVisualLayer(uint new_layer);
	uint				GetVisualLayer()const;
	virtual void		SetLogicalLayer(uint new_layer);
	uint				GetLogicalLayer()const;
	UI_TYPE				GetUItype()const;

	//Active/Deactivate functions
	void				Activate();
	void				Desactivate();
	bool				GetActiveState()const;

	//Active/Deactivate childs
	void				ActivateChilds();
	void				DesactivateChids();

	//Tree functions
	void				AddChild(UI_Element* child, uint start_layer = 0);
	void				Delete_Child(UI_Element* child);
	void				Delete_Child(uint index);
	UI_Element*			SetParent(const UI_Element* parent);
	UI_Element*			GetParent()const;

	// Input target functions
	void				SetInputTarget(j1Module* target);
	j1Module*			GetInputTarget()const;

	// Next/Prev in focus
	void				SetNextInFocus(const UI_Element* target);
	UI_Element*			GetNextInFocus()const;
	void				SetPrevInFocus(const UI_Element* target);
	UI_Element*			GetPrevInFocus()const;

	//Block functions
	virtual void		Block();
	virtual void		UnBlock();
	bool				GetBlockState();
};

#endif // __UI_ELEMENT__