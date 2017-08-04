#include "Scene.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Gui.h"
#include "j1Animator.h"
#include "j1FileSystem.h"
#include "j1InputManager.h"
#include "j1Audio.h"
#include "j1Physics.h"
#include "j1EntitiesManager.h"

#include "Parallax.h"
#include "UI_Element.h"
#include "UI_Button.h"
#include "UI_Image.h"
#include "UI_Scroll_Bar.h"

#include "MainMenu.h"
#include "Tutorial.h"
#include "Endless.h"

// Constructor ==================================
Scene::Scene() : j1Module()
{

}

// Destructor ===================================
Scene::~Scene()
{

}

// Game Loop ====================================
void Scene::Init()
{
	active = enabled = base_enabled = false;
}

bool Scene::Enable()
{
	enabled = base_enabled = true;
	active = false;

	//UI Activation
	menu_branch->Activate();
	menu_branch->ActivateChilds();
	settings_menu->Desactivate();
	settings_menu->DesactivateChids();
	audio_menu->Desactivate();
	audio_menu->DesactivateChids();
	video_menu->Desactivate();
	video_menu->DesactivateChids();
	death_menu->Desactivate();
	death_menu->DesactivateChids();

	return true;
}

void Scene::Disable()
{
	//UI Deactivation
	settings_exit_scene_button->UnBlock();
	death_end_button->UnBlock();
	death_reset_button->UnBlock();
	menu_branch->Desactivate();
	menu_branch->DesactivateChids();

	//Release parallax system
	if (front_parallax != nullptr)RELEASE(front_parallax);
	if (mid_parallax != nullptr)RELEASE(mid_parallax);
	if (back_parallax != nullptr)RELEASE(back_parallax);

	//Release scene physic system
	if (floor_collider != nullptr)RELEASE(floor_collider);

	//Delete all the current entities in the scene
	App->entities_manager->DeleteCurrentEntities();

	active = enabled = base_enabled = false;
}

void Scene::RestartScene()
{

}

bool Scene::Awake(pugi::xml_node& data_node)
{
	LOG("Loading %s Definition Doc",name.c_str());

	//Load the data document used to generate the scene when is enabled
	App->fs->LoadXML(data_node.child("data_folder").child_value(), &data_doc);
	if (data_doc.root() == NULL)
	{
		LOG("Error Loading %s Doc", name.c_str());
	}
	return true;
}

bool Scene::Start()
{
	//Build scene UI --------

	//Scene Branch ------------------------------
	menu_branch = App->gui->GenerateUI_Element(UI_TYPE::UNDEFINED);
	menu_branch->SetInputTarget(this);
	menu_branch->Desactivate();

	//Settings Button
	settings_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	settings_button->SetBox({ 1500,50,50,50 });
	settings_button->SetTexOFF({ 0,199,50,50 }, ATLAS);
	settings_button->SetTexOVER({ 0,598,50,50 }, ATLAS_TEST);
	settings_button->SetTexON({ 0,598,50,50 }, ATLAS);
	settings_button->SetInputTarget(this);
	settings_button->SetParent(menu_branch);
	settings_button->Desactivate();
	menu_branch->AddChild(settings_button);
	// ------------------------------------------

	//Build Setting Menu ------------------------

	//Settings menu base
	settings_menu = (UI_Image*)App->gui->GenerateUI_Element(UI_TYPE::IMG);
	settings_menu->SetInputTarget(this);
	settings_menu->SetParent(settings_button);
	settings_menu->SetBox({ 100,100,500,700 });
	settings_menu->ChangeTextureId(ATLAS);
	settings_menu->ChangeTextureRect({ 100,100,500,700 });
	settings_menu->Desactivate();
	settings_menu->DesactivateChids();
	menu_branch->AddChild(settings_menu);

	//Quit button
	settings_quit_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	settings_quit_button->SetInputTarget(this);
	settings_quit_button->SetParent(settings_menu);
	settings_quit_button->SetBox({ 450,20,130,95 });
	settings_quit_button->SetTexOFF({ 0,766,126,93 }, ATLAS);
	settings_quit_button->SetTexON({ 0,266,126,93 }, ATLAS);
	settings_quit_button->SetTexOVER({ 0,266,126,93 }, ATLAS);
	settings_quit_button->Desactivate();
	settings_menu->AddChild(settings_quit_button);

	//Audio button
	settings_audio_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	settings_audio_button->SetBox({ 200,200,570,170 });
	settings_audio_button->SetTexOFF({ 0,199,564,165 }, ATLAS);
	settings_audio_button->SetTexOVER({ 0,598,564,165 }, ATLAS_TEST);
	settings_audio_button->SetTexON({ 0,598,564,165 }, ATLAS);
	settings_audio_button->SetInputTarget(this);
	settings_audio_button->SetParent(menu_branch);
	settings_audio_button->Desactivate();
	settings_menu->AddChild(settings_audio_button);

	//Video button
	settings_video_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	settings_video_button->SetBox({ 200,400,570,170 });
	settings_video_button->SetTexOFF({ 0,199,564,165 }, ATLAS);
	settings_video_button->SetTexOVER({ 0,598,564,165 }, ATLAS_TEST);
	settings_video_button->SetTexON({ 0,598,564,165 }, ATLAS);
	settings_video_button->SetInputTarget(this);
	settings_video_button->SetParent(menu_branch);
	settings_video_button->Desactivate();
	settings_menu->AddChild(settings_video_button);

	//Exit button
	settings_exit_scene_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	settings_exit_scene_button->SetBox({ 200,600,570,170 });
	settings_exit_scene_button->SetTexOFF({ 0,199,564,165 }, ATLAS);
	settings_exit_scene_button->SetTexOVER({ 0,598,564,165 }, ATLAS_TEST);
	settings_exit_scene_button->SetTexON({ 0,598,564,165 }, ATLAS);
	settings_exit_scene_button->SetInputTarget(this);
	settings_exit_scene_button->SetParent(menu_branch);
	settings_exit_scene_button->Desactivate();
	settings_menu->AddChild(settings_exit_scene_button);
	// ------------------------------------------

	//Build Audio Menu --------------------------
	//Audio menu base
	audio_menu = (UI_Image*)App->gui->GenerateUI_Element(UI_TYPE::IMG);
	audio_menu->SetInputTarget(this);
	audio_menu->SetParent(settings_button);
	audio_menu->SetBox({ 100,100,500,700 });
	audio_menu->ChangeTextureId(ATLAS);
	audio_menu->ChangeTextureRect({ 100,100,500,700 });
	audio_menu->Desactivate();
	audio_menu->DesactivateChids();
	menu_branch->AddChild(audio_menu);
	audio_menu->SetLayer(0);

	//Quit button
	audio_quit_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	audio_quit_button->SetInputTarget(this);
	audio_quit_button->SetParent(settings_menu);
	audio_quit_button->SetBox({ 450,20,130,95 });
	audio_quit_button->SetTexOFF({ 0,766,126,93 }, ATLAS);
	audio_quit_button->SetTexON({ 0,266,126,93 }, ATLAS);
	audio_quit_button->SetTexOVER({ 0,266,126,93 }, ATLAS);
	audio_quit_button->Desactivate();
	audio_menu->AddChild(audio_quit_button);

	//Master scroll
	master_audio_scroll = (UI_Scroll_Bar*)App->gui->GenerateUI_Element(UI_TYPE::SCROLL_BAR);
	master_audio_scroll->SetScrollBarType(SCROLL_BAR_TYPE::LATERAL_BAR);
	master_audio_scroll->SetBox({ 50,50,400,100 });
	master_audio_scroll->SetScrollMaxValue(MAX_VOLUME);
	master_audio_scroll->SetScrollableBack({ 40,50 }, { 0,0,350,50 }, ATLAS);
	master_audio_scroll->SetScrollableItem({ 50,40 }, { 0,500,30,70 }, ATLAS);
	master_audio_scroll->SetScrollValue(INITIAL_VOLUME);
	master_audio_scroll->Desactivate();
	master_audio_scroll->SetInputTarget(this);
	audio_menu->AddChild(master_audio_scroll);

	//Music scroll
	music_audio_scroll = (UI_Scroll_Bar*)App->gui->GenerateUI_Element(UI_TYPE::SCROLL_BAR);
	music_audio_scroll->SetScrollBarType(SCROLL_BAR_TYPE::LATERAL_BAR);
	music_audio_scroll->SetBox({ 50,200,400,100 });
	music_audio_scroll->SetScrollMaxValue(INITIAL_VOLUME);
	music_audio_scroll->SetScrollableBack({ 40,50 }, { 0,0,350,50 }, ATLAS);
	music_audio_scroll->SetScrollableItem({ 10,40 }, { 0,500,30,70 }, ATLAS);
	music_audio_scroll->SetScrollValue(INITIAL_VOLUME);
	music_audio_scroll->Desactivate();
	music_audio_scroll->SetInputTarget(this);
	audio_menu->AddChild(music_audio_scroll);

	//FX scroll
	fx_audio_scroll = (UI_Scroll_Bar*)App->gui->GenerateUI_Element(UI_TYPE::SCROLL_BAR);
	fx_audio_scroll->SetScrollBarType(SCROLL_BAR_TYPE::LATERAL_BAR);
	fx_audio_scroll->SetBox({ 50,350,400,100 });
	fx_audio_scroll->SetScrollMaxValue(INITIAL_VOLUME);
	fx_audio_scroll->SetScrollableBack({ 40,50 }, { 0,0,350,50 }, ATLAS);
	fx_audio_scroll->SetScrollableItem({ 10,40 }, { 0,500,30,70 }, ATLAS);
	fx_audio_scroll->SetScrollValue(INITIAL_VOLUME);
	fx_audio_scroll->Desactivate();
	fx_audio_scroll->SetInputTarget(this);
	audio_menu->AddChild(fx_audio_scroll);

	//Build Video Menu --------------------------
	//Video menu base
	video_menu = (UI_Image*)App->gui->GenerateUI_Element(UI_TYPE::IMG);
	video_menu->SetInputTarget(this);
	video_menu->SetParent(settings_button);
	video_menu->SetBox({ 100,100,500,700 });
	video_menu->ChangeTextureId(ATLAS);
	video_menu->ChangeTextureRect({ 100,100,500,700 });
	video_menu->Desactivate();
	video_menu->DesactivateChids();
	menu_branch->AddChild(video_menu);
	video_menu->SetLayer(0);

	//Quit button
	video_quit_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	video_quit_button->SetInputTarget(this);
	video_quit_button->SetParent(settings_menu);
	video_quit_button->SetBox({ 450,20,130,95 });
	video_quit_button->SetTexOFF({ 0,766,126,93 }, ATLAS);
	video_quit_button->SetTexON({ 0,266,126,93 }, ATLAS);
	video_quit_button->SetTexOVER({ 0,266,126,93 }, ATLAS);
	video_quit_button->Desactivate();
	video_menu->AddChild(video_quit_button);

	//Vsync button
	vsync_video_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	vsync_video_button->SetBox({ 200,200,570,170 });
	vsync_video_button->SetTexOFF({ 0,199,564,165 }, ATLAS);
	vsync_video_button->SetTexOVER({ 0,598,564,165 }, ATLAS_TEST);
	vsync_video_button->SetTexON({ 0,598,564,165 }, ATLAS);
	vsync_video_button->SetInputTarget(this);
	vsync_video_button->SetParent(menu_branch);
	vsync_video_button->Desactivate();
	video_menu->AddChild(vsync_video_button);

	//Fullscreen button
	fullscreen_video_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	fullscreen_video_button->SetBox({ 200,350,570,170 });
	fullscreen_video_button->SetTexOFF({ 0,199,564,165 }, ATLAS);
	fullscreen_video_button->SetTexOVER({ 0,598,564,165 }, ATLAS_TEST);
	fullscreen_video_button->SetTexON({ 0,598,564,165 }, ATLAS);
	fullscreen_video_button->SetInputTarget(this);
	fullscreen_video_button->SetParent(menu_branch);
	fullscreen_video_button->Desactivate();
	video_menu->AddChild(fullscreen_video_button);

	//Build death menu --------------------------
	//Death menu base
	death_menu = (UI_Image*)App->gui->GenerateUI_Element(UI_TYPE::IMG);
	death_menu->SetInputTarget(this);
	death_menu->SetParent(menu_branch);
	death_menu->SetBox({ 100,100,500,700 });
	death_menu->ChangeTextureId(ATLAS);
	death_menu->ChangeTextureRect({ 100,100,500,700 });
	death_menu->Desactivate();
	death_menu->DesactivateChids();
	menu_branch->AddChild(death_menu);
	death_menu->SetLayer(0);

	//Death Quit
	death_end_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	death_end_button->SetInputTarget(this);
	death_end_button->SetParent(settings_menu);
	death_end_button->SetBox({ 200,450,570,170 });
	death_end_button->SetTexOFF({ 0,199,564,165 }, ATLAS);
	death_end_button->SetTexOVER({ 0,598,564,165 }, ATLAS_TEST);
	death_end_button->SetTexON({ 0,598,564,165 }, ATLAS);
	death_end_button->Desactivate();
	death_menu->AddChild(death_end_button);

	//Death Reset
	death_reset_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	death_reset_button->SetInputTarget(this);
	death_reset_button->SetParent(settings_menu);
	death_reset_button->SetBox({ 200,200,570,170 });
	death_reset_button->SetTexOFF({ 0,199,564,165 }, ATLAS);
	death_reset_button->SetTexOVER({ 0,598,564,165 }, ATLAS_TEST);
	death_reset_button->SetTexON({ 0,598,564,165 }, ATLAS);
	death_reset_button->Desactivate();
	death_menu->AddChild(death_reset_button);

	//Add the built branch at the GUI 
	App->gui->PushScreen(menu_branch);

	return true;
}

bool Scene::Update(float dt)
{
	App->gui->CalculateUpperElement(menu_branch);

	//EXIT --------------------------------------
	if (App->input_manager->GetEvent(ESCAPE) == INPUT_DOWN)
	{
		App->ActiveMainMenu();
	}
	// ------------------------------------------

	return true;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	
	if (front_parallax != nullptr) RELEASE(front_parallax);
	if (mid_parallax != nullptr)RELEASE(mid_parallax);
	if (back_parallax != nullptr)RELEASE(back_parallax);
	
	if (floor_collider != nullptr)RELEASE(floor_collider);

	data_doc.reset();

	return true;
}

// GUI Functions ================================
void Scene::GUI_Input(UI_Element * target, GUI_INPUT input)
{
	int x_vel, y_vel;
	App->input->GetMouseMotion(x_vel, y_vel);

	if (input == GUI_INPUT::MOUSE_LEFT_BUTTON_DOWN)
	{
		//Main Buttons ----------------
		if (target == settings_button)
		{
			//Deactivate start, settings & quit buttons
			settings_button->Desactivate();

			//Activate settings menu and all the childs
			settings_menu->Activate();
			settings_menu->ActivateChilds();
			settings_audio_button->DesactivateChids();
			settings_video_button->DesactivateChids();

			//Set the correct input target in the new menu
			App->gui->ItemSelected = settings_audio_button;

			//Set the correct app context
			App->app_context = PAUSE_CONTEXT;
		}


		//Settings Buttons ------------
		else if (target == settings_quit_button)
		{
			//Deactivate settings menu
			settings_menu->Desactivate();
			settings_menu->DesactivateChids();
			//Activate menu buttons
			settings_button->Activate();

			//Set the correct app context
			App->app_context = IN_GAME_CONTEXT;

			//Set the correct input target in the new menu
			App->gui->ItemSelected = nullptr;
		}
		else if (target == settings_audio_button)
		{
			//Deactivate settings menu
			settings_menu->Desactivate();
			settings_menu->DesactivateChids();
			
			//Activate audio menu
			audio_menu->Activate();
			audio_menu->ActivateChilds();

			//Set the correct input target in the new menu
			App->gui->ItemSelected = master_audio_scroll;
		}
		else if (target == settings_video_button)
		{
			//Deactivate settings menu
			settings_menu->Desactivate();
			settings_menu->DesactivateChids();
			
			//Activate audio menu
			video_menu->Activate();
			video_menu->ActivateChilds();

			//Set the correct input target in the new menu
			App->gui->ItemSelected = vsync_video_button;

		}
		else if (target == settings_exit_scene_button && !settings_exit_scene_button->GetBlockState())
		{
			settings_exit_scene_button->Block();
			App->ActiveMainMenu();
		}
		//Audio Buttons ---------------
		else if (target == audio_quit_button)
		{
			//Deactivate audio menu
			audio_menu->Desactivate();
			audio_menu->DesactivateChids();
			
			//Activate settings menu
			settings_menu->Activate();
			settings_menu->ActivateChilds();

			//Set the correct input target in the new menu
			App->gui->ItemSelected = settings_audio_button;
		}
		//Video Buttons ---------------
		else if (target == video_quit_button)
		{
			//Deactivate audio menu
			video_menu->Desactivate();
			video_menu->DesactivateChids();
			
			//Activate settings menu
			settings_menu->Activate();
			settings_menu->ActivateChilds();

			//Set the correct input target in the new menu
			App->gui->ItemSelected = settings_audio_button;
		}
		else if (target == fullscreen_video_button)
		{
			App->win->ChangeFullscreen();
		}
		else if (target == vsync_video_button)
		{
			App->render->ChangeVSYNCstate(!App->render->vsync);
		}
		//Death Buttons ---------------
		else if (target == death_end_button)
		{
			death_end_button->Block();
			App->ActiveMainMenu();
		}
		else if (target == death_reset_button)
		{
			death_reset_button->Block();
			App->GetCurrentScene()->RestartScene();

			//Set the correct input target in the new menu
			App->gui->ItemSelected = nullptr;
		}
	}
	else if (input == GUI_INPUT::MOUSE_LEFT_BUTTON_REPEAT)
	{
		//Audio Scrolls ---------------
		if (target == master_audio_scroll)
		{
			//Master
			App->main_menu->master_audio_scroll->MoveScroll(y_vel, x_vel);
			App->endless->master_audio_scroll->MoveScroll(y_vel, x_vel);
			App->tutorial->master_audio_scroll->MoveScroll(y_vel, x_vel);
			App->audio->SetMasterVolume(master_audio_scroll->GetValue());
			//Music
			App->main_menu->music_audio_scroll->SetScrollMaxValue(master_audio_scroll->GetValue());
			App->endless->music_audio_scroll->SetScrollMaxValue(master_audio_scroll->GetValue());
			App->tutorial->music_audio_scroll->SetScrollMaxValue(master_audio_scroll->GetValue());
			App->main_menu->music_audio_scroll->RecalculateScrollValue();
			App->endless->music_audio_scroll->RecalculateScrollValue();
			App->tutorial->music_audio_scroll->RecalculateScrollValue();
			App->audio->SetMusicVolume(music_audio_scroll->GetValue());
			//FX
			App->main_menu->fx_audio_scroll->SetScrollMaxValue(master_audio_scroll->GetValue());
			App->endless->fx_audio_scroll->SetScrollMaxValue(master_audio_scroll->GetValue());
			App->tutorial->fx_audio_scroll->SetScrollMaxValue(master_audio_scroll->GetValue());
			App->main_menu->fx_audio_scroll->RecalculateScrollValue();
			App->endless->fx_audio_scroll->RecalculateScrollValue();
			App->tutorial->fx_audio_scroll->RecalculateScrollValue();
			App->audio->SetFXVolume(fx_audio_scroll->GetValue());
		}
		else if (target == music_audio_scroll)
		{
			App->main_menu->music_audio_scroll->MoveScroll(y_vel, x_vel);
			App->endless->music_audio_scroll->MoveScroll(y_vel, x_vel);
			App->tutorial->music_audio_scroll->MoveScroll(y_vel, x_vel);
			App->audio->SetMusicVolume(music_audio_scroll->GetValue());
		}
		else if (target == fx_audio_scroll)
		{
			App->main_menu->fx_audio_scroll->MoveScroll(y_vel, x_vel);
			App->endless->fx_audio_scroll->MoveScroll(y_vel, x_vel);
			App->tutorial->fx_audio_scroll->MoveScroll(y_vel, x_vel);
			App->audio->SetFXVolume(fx_audio_scroll->GetValue());
		}
	}
}

void Scene::GUI_ControllerInput(INPUT_EVENT input_event)
{
	UI_Element* target = App->gui->ItemSelected;
	if (target == nullptr)LOG("ERROR!");

}

UI_Element * Scene::GetCorrectItemToSelect() const
{
	UI_Element* item_to_select = nullptr;
	
	//Setting Menu Case -----
	if (settings_menu->GetActiveState())
	{
		item_to_select = settings_audio_button;
	}

	//Audio Menu Case -------
	else if (audio_menu->GetActiveState())
	{
		item_to_select = master_audio_scroll;
	}

	//Video Menu Case -------
	else if (video_menu->GetActiveState())
	{
		item_to_select = vsync_video_button;
	}

	//Death Menu Case -------
	else if (death_menu->GetActiveState())
	{
		item_to_select = death_reset_button;
	}

	return item_to_select;
}

// Functionality ================================
void Scene::UpdateParallax(float disp)
{
	if(mid_parallax != nullptr)mid_parallax->Displace(disp * MID_PARALLAX_VAL * App->GetDT());
	if (back_parallax != nullptr)back_parallax->Displace(disp * BACK_PARALLAX_VAL * App->GetDT());
}

void Scene::PlayerDeathMode()
{
	//Deactivate scene UI
	settings_menu->Desactivate();
	settings_menu->DesactivateChids();
	settings_button->Desactivate();

	//Activate death menu
	death_menu->Activate();
	death_menu->ActivateChilds();
}
