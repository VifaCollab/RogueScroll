#ifndef __j1ENTITIES_MANAGER_H__
#define __j1ENTITIES_MANAGER_H__

#include "j1Module.h"
#include "BaseEntities.h"
#include "Items.h"
#include "Creatures.h"
#include "j1Timer.h"

#include "UI_String.h"

class j1EntitiesManager : public j1Module
{
public:

	j1EntitiesManager();
	~j1EntitiesManager();

public:

	// Game Loop ------------
	void Init();
	bool Enable();
	void Disable();

	// Called before render is available
	bool Awake(pugi::xml_node& node);

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Collisions functions
	void BeginCollision(PhysBody* A, PhysBody* B);
	void BeginSensorCollision(PhysBody* A, PhysBody* B);

private:

	// Data used during the segmented enable process
	pugi::xml_document	definitions_doc;
	pugi::xml_node		current_enable_group;
	pugi::xml_node		current_enble_node;
	ENTITY_TYPE			current_entity_type = NO_ENTITY;

	//Add definitions methods
	void AddItemDefinition(const pugi::xml_node* data_node);
	void AddCreatureDefinition(const pugi::xml_node* data_node);
	void AddCreatureEvoTemplate(const pugi::xml_node* data_node);

	// Vectors with all the definitions
	std::vector<Creature*>				creatures_defs;
	std::vector<Evolution_Template*>	creatures_evo_templates;
	std::vector<Item*>					items_defs;

	// List with all the alive creatures
	std::list<Entity*>		current_entities;
	// List with all the entities ready to be cleared
	std::list<Entity*>		entitites_to_clear;
	//List with all the entities ready to be deleted
	std::list<Entity*>		entitites_to_delete;
	//Vector with all the entities generated in the last frame
	std::vector<Entity*>	entities_to_add;

	//Entity selected data
	Entity*		entity_selected = nullptr;
	UI_String	entity_sel_string;
	j1Timer		entity_sel_timer;

public:

	//Entities UI data ------
	uint				entities_ui_visual_layer = 0;
	uint				selected_str_margin = 0;
	uint				selected_update_rate = 0;
	uint				bar_life_margin = 0;
	UI_Progressive_Bar	ally_bar_life_template;
	UI_Progressive_Bar	enemy_bar_life_template;

	//Enums Methods ---------
	ENTITY_TYPE				StrToEntityType(const char* str)const;
	DIPLOMACY				StrToDiplomacy(const char* str)const;
	CREATURE_TYPE			StrToCreatureType(const char* str)const;
	ITEM_TYPE				StrToItemType(const char* str)const;
	DIRECTION				StrToDirection(const char* str)const;
	ACTION_TYPE				StrToActionType(const char* str)const;
	std::vector<ITEM_TYPE>	TokenStrToItemTypes(const char* str)const;
	SDL_Color				TokenStrToColor(const char* str)const;
	SDL_Rect				TokenStrToRect(const char* str)const;

	// Functionality --------
	Creature*	GenerateCreature(CREATURE_TYPE creature_type, bool generate_body = true);
	Item*		GenerateItem(ITEM_TYPE item_type, bool generate_body = true);
	
	bool		LevelUpCreature(Creature* target);

	void		AddEntity(const Entity* target);
	void		ClearEntity(Entity* target);
	void		DeleteEntity(Entity* target);
	void		DeleteCurrentEntities();
};
#endif