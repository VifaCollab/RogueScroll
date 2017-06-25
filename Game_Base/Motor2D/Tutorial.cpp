#include "Tutorial.h"

#include "j1App.h"
#include "j1Physics.h"
#include "j1Audio.h"
#include "j1InputManager.h"
#include "j1EntitiesManager.h"
#include "j1Input.h"
#include "j1Player.h"

#include "Parallax.h"
#include "UI_Element.h"
#include "UI_Button.h"
#include "UI_Image.h"
#include "UI_Scroll_Bar.h"

// Constructor ==================================
Tutorial::Tutorial()
{
	name = "tutorial";
}

// Destructor ===================================
Tutorial::~Tutorial()
{

}

// Game Loop ====================================
bool Tutorial::Enable()
{
	if (!base_enabled)
	{
		//UI Activation ---------------------------------------
		menu_branch->Activate();
		menu_branch->ActivateChilds();
		settings_menu->Desactivate();
		settings_menu->DesactivateChids();
		audio_menu->Desactivate();
		audio_menu->DesactivateChids();
		video_menu->Desactivate();
		video_menu->DesactivateChids();

		//Map build -------------------------------------------
		//Floor -----------------
		current_enable_node = data_doc.root().first_child().child("floor_collider");

		floor_collider = App->physics->CreateRectangle(
			current_enable_node.attribute("x").as_int(),
			current_enable_node.attribute("y").as_int(),
			current_enable_node.attribute("w").as_int(),
			current_enable_node.attribute("h").as_int(),
			COLLISION_TYPE::MAP_COLLISION, BODY_TYPE::MAP_BODY
		);
		floor_collider->body->SetType(b2BodyType::b2_staticBody);
		floor_collider->body->GetFixtureList()->SetFriction(current_enable_node.attribute("friction").as_float());

		//End Trigger -----------
		current_enable_node = data_doc.root().first_child().child("end_trigger");

		end_trigger = App->physics->CreateRectangle(
			current_enable_node.attribute("x").as_int(),
			current_enable_node.attribute("y").as_int(),
			current_enable_node.attribute("w").as_int(),
			current_enable_node.attribute("h").as_int(),
			COLLISION_TYPE::MAP_COLLISION, BODY_TYPE::MAP_BODY
		);
		end_trigger->body->SetType(b2BodyType::b2_staticBody);
		end_trigger->body->GetFixtureList()->SetSensor(true);
		end_trigger->listener = this;

		//Front Parallax --------
		current_enable_node = data_doc.root().first_child().child("front_pllx");
		
		front_parallax = new Parallax(current_enable_node.attribute("cells").as_uint());
		front_parallax->SetTexture(App->tex->Load(current_enable_node.attribute("tex_folder").as_string()));
		front_parallax->SetTextureRect({ 
			current_enable_node.attribute("tex_x").as_int(),
			current_enable_node.attribute("tex_y").as_int(),
			current_enable_node.attribute("tex_w").as_int(),
			current_enable_node.attribute("tex_h").as_int()
		});
		front_parallax->SetPosition(0, current_enable_node.attribute("pos_y").as_int());

		//Mid Parallax ----------
		current_enable_node = data_doc.root().first_child().child("mid_pllx");
		
		mid_parallax = new Parallax(current_enable_node.attribute("cells").as_uint());
		mid_parallax->SetTexture(App->tex->Load(current_enable_node.attribute("tex_folder").as_string()));
		mid_parallax->SetTextureRect({
			current_enable_node.attribute("tex_x").as_int(),
			current_enable_node.attribute("tex_y").as_int(),
			current_enable_node.attribute("tex_w").as_int(),
			current_enable_node.attribute("tex_h").as_int()
		});
		mid_parallax->SetPosition(0, current_enable_node.attribute("pos_y").as_int());

		//Back Parallax ---------
		current_enable_node = data_doc.root().first_child().child("back_pllx");
		
		back_parallax = new Parallax(current_enable_node.attribute("cells").as_uint());
		back_parallax->SetTexture(App->tex->Load(current_enable_node.attribute("tex_folder").as_string()));
		back_parallax->SetTextureRect({
			current_enable_node.attribute("tex_x").as_int(),
			current_enable_node.attribute("tex_y").as_int(),
			current_enable_node.attribute("tex_w").as_int(),
			current_enable_node.attribute("tex_h").as_int()
		});
		back_parallax->SetPosition(0, current_enable_node.attribute("pos_y").as_int());

		//Prepare tutorial entities ---------------------------
		current_enable_node = data_doc.root().first_child().child("player_avatar");
		
		App->player->avatar->SetPosition(current_enable_node.attribute("pos_x").as_float(), current_enable_node.attribute("pos_y").as_float());

		//Set enable node root at null
		current_enable_node = current_enable_node.child("none");

		//Set boolean to check that base data has been loaded
		base_enabled = true;

		return false; /*This divide the first load step in one frame*/
	}
	else
	{
		j1Timer	enable_timer;

		//If the node is null focus it to the first scene entity
		if (current_enable_node.root() == NULL)
		{
			current_enable_node = data_doc.root().first_child().child("entities").first_child();
		}

		while (current_enable_node.root() != NULL)
		{

			ENTITY_TYPE entity_type = App->entities_manager->StrToEntityType(current_enable_node.attribute("entity_type").as_string());
			Entity* new_entity = nullptr;

			switch (entity_type)
			{
			case CREATURE:
			{
				CREATURE_TYPE creature_type = App->entities_manager->StrToCreatureType(current_enable_node.attribute("creature_type").as_string());
				new_entity = App->entities_manager->GenerateCreature(creature_type);
				
				break;
			}
			case ITEM:
				ITEM_TYPE item_type = App->entities_manager->StrToItemType(current_enable_node.attribute("item_type").as_string());
				new_entity = App->entities_manager->GenerateItem(item_type);
								
				break;
			}

			//Set the new entity scene data
			/*Position*/	new_entity->SetPosition(current_enable_node.attribute("pos_x").as_float(), current_enable_node.attribute("pos_y").as_float());

			//Add the generated at the scene entities vector
			entities_generated.push_back(new_entity);

			//Focus the next entity node
			current_enable_node = current_enable_node.next_sibling();

			if (enable_timer.Read() > TIME_TO_ENABLE)return false;
		}
	}

	//Play scene music
	App->audio->PlayMusic(MUSIC_ID::MUSIC_IN_GAME);

	active = enabled = true;

	return true;
}

void Tutorial::Disable()
{
	LOG("Disabling Tutorial");

	//UI Deactivation
	settings_exit_scene_button->UnBlock();
	menu_branch->Desactivate();
	menu_branch->DesactivateChids();

	//Release parallax system
	if (front_parallax != nullptr) RELEASE(front_parallax);
	if (mid_parallax != nullptr)RELEASE(mid_parallax);
	if (back_parallax != nullptr)RELEASE(back_parallax);

	//Release tutorial physic system
	if (floor_collider != nullptr)RELEASE(floor_collider);
	if (end_trigger != nullptr)RELEASE(end_trigger);

	//Delete all the generated entities in the scene
	uint size = entities_generated.size();
	for (uint k = 0; k < size; k++)
	{
		App->entities_manager->DeleteEntity(entities_generated[k]);
	}
	entities_generated.clear();

	active = enabled = base_enabled = false;
}

bool Tutorial::Update(float dt)
{
	App->gui->CalculateUpperElement(menu_branch);

	//Blit scene Parallax -----------------------
	back_parallax->Draw(-30);
	mid_parallax->Draw(-40);
	front_parallax->Draw(-30);
	// ------------------------------------------

	//EXIT --------------------------------------
	if (App->input_manager->GetEvent(ESCAPE) == INPUT_DOWN)
	{
		if (settings_menu->GetActiveState())
		{
			//Deactivate settings menu
			settings_menu->Desactivate();
			settings_menu->DesactivateChids();
			//Activate menu buttons
			settings_button->Activate();
		}
		else if (video_menu->GetActiveState() || audio_menu->GetActiveState())
		{
			//Deactivate settings sub menus
			video_menu->Desactivate();
			video_menu->DesactivateChids();
			audio_menu->Desactivate();
			audio_menu->DesactivateChids();
			//Activate settings menu
			settings_menu->Activate();
			settings_menu->ActivateChilds();
		}
		else
		{
			//Deactivate settings button
			settings_button->Desactivate();
			//Activate settings menu
			settings_menu->Activate();
			settings_menu->ActivateChilds();
		}
	}
	// ------------------------------------------

	//TEST ZONE ---------------------------------
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		Item* jar = App->entities_manager->GenerateItem(JAR_ITEM);
		jar->GetBody()->SetPosition(App->input->GetMouseX() - App->render->camera.x, App->input->GetMouseY() - App->render->camera.y);
	}
	// ------------------------------------------
	return true;
}

bool Tutorial::CleanUp()
{
	LOG("Freeing Tutorial");

	if (front_parallax != nullptr) delete front_parallax;
	if (mid_parallax != nullptr)delete mid_parallax;
	if (back_parallax != nullptr)delete back_parallax;

	if (floor_collider != nullptr)delete floor_collider;
	if (end_trigger != nullptr)delete end_trigger;

	data_doc.reset();

	return true;
}

void Tutorial::BeginSensorCollision(PhysBody * A, PhysBody * B)
{
	if (A == end_trigger)
	{
		App->ActiveEndless();
	}
}
