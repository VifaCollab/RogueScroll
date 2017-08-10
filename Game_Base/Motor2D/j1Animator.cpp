#include "j1Animator.h"

#include <time.h>
#include <stdlib.h>

#include "j1App.h"
#include "j1FileSystem.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "SDL/include/SDL_rect.h"
#include "p2Log.h"
#include "j1EntitiesManager.h"

///Animation Sprite Class -----------------------
//Constructor ===============
Sprite::Sprite(const SDL_Rect & frame, const iPoint & pivot, int z_cord, uint opacity) : frame(frame), pivot(pivot), z_cord(z_cord), opacity(opacity)
{

}

//Destructor ================
Sprite::~Sprite()
{
}

//Functionality =============
const SDL_Rect * Sprite::GetFrame() const
{
	return &frame;
}
int Sprite::GetXpivot() const
{
	return pivot.x;
}
int Sprite::GetYpivot() const
{
	return pivot.y;
}
int Sprite::GetZ_cord() const
{
	return z_cord;
}
uint Sprite::GetOpacity() const
{
	return opacity;
}
/// ---------------------------------------------



///Animation Class ------------------------------
//Constructor =========================
Animation::Animation()
{

}

//Destructor ==========================
Animation::~Animation()
{
	sprites.clear();
	texture = nullptr;
	//This texture can't be unloaded
}


//Functionality =======================
void Animation::ReleaseTexture()
{
	App->tex->UnLoad(texture);
}

void Animation::SetTexture(const SDL_Texture * tex)
{
	texture = (SDL_Texture*)tex;
}

void Animation::SetLoop(bool loop_state)
{
	loop = loop_state;
}

void Animation::SetCurrentFrame(uint curr)
{
	if (curr < sprites.size())	current_frame = curr;
	else current_frame = sprites.size() - 1;
}

void Animation::SetSpeed(uint new_speed)
{
	speed = new_speed;
}

void Animation::SetId(uint id)
{
	enum_id = id;
}

SDL_Texture * Animation::GetTexture() const
{
	return texture;
}

bool Animation::GetLoop() const
{
	return loop;
}

uint Animation::GetCurrentFrame() const
{
	return current_frame;
}

uint Animation::GetSpeed() const
{
	return speed;
}

const Sprite* Animation::GetCurrentSprite()
{
	if (current_frame == -1)return &sprites[sprites.size() - 1];

	//if (App->paused)return &sprites[(int)current_frame];

	current_frame = (float)floor(timer.Read() / speed);
	if (current_frame >= sprites.size())
	{
		if (loop)
		{
			loops++;
			current_frame = 0;
			timer.Start();
		}
		else
		{
			loops = 0;
			current_frame = -1;
			return &sprites[sprites.size() - 1];
		}
	}

	return &sprites[(int)current_frame];
}

const std::vector<Sprite>* Animation::GetAllSprites() const
{
	return &sprites;
}

uint Animation::GetId() const
{
	return enum_id;
}

bool Animation::IsEnd()
{
	return (current_frame == -1 && !loop);
}

void Animation::Reset()
{
	timer.Start();
	current_frame = 0;
}

void Animation::AddSprite(const SDL_Rect & rect, const iPoint & point, const int z, const uint opacity)
{
	sprites.push_back(Sprite(rect, point, z, opacity));
}
/// ---------------------------------------------


/// Animation Block Class -----------------------
//Constructor =========================
Animation_Block::Animation_Block(uint enum_id) :enum_id(enum_id)
{

}

//Destructor ==========================
Animation_Block::~Animation_Block()
{
	ClearAnimationBlocks();

	if (animation != nullptr)RELEASE(animation);
}

//Functionality =======================
void Animation_Block::ClearAnimationBlocks()
{
	uint size = childs.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(childs[k]);
	}

	if (animation != nullptr)
	{
		animation->ReleaseTexture();
		RELEASE(animation);
	}
}

void Animation_Block::SetId(uint id)
{
	enum_id = id;
}

uint Animation_Block::GetId() const
{
	return enum_id;
}

Animation* Animation_Block::GetAnimation() const
{
	return animation;
}

Animation_Block * Animation_Block::GetBlock(int index) const
{
	return childs.at(index);
}

uint Animation_Block::GetChildsNum() const
{
	return childs.size();
}

Animation_Block * Animation_Block::SearchId(uint id) const
{
	uint size = childs.size();
	for (uint k = 0; k < size; k++)
	{
		if (childs[k]->GetId() == id)return childs[k];
	}

	return nullptr;
}

void Animation_Block::SetAnimation(const Animation * new_animation)
{
	if (new_animation != nullptr)animation = (Animation*)new_animation;
}

void Animation_Block::AddAnimationBlock(Animation_Block* new_animation_block)
{
	if (new_animation_block != nullptr)childs.push_back(new_animation_block);
}
/// ---------------------------------------------

//Animator Module -------------------------------
//Constructor =========================
j1Animator::j1Animator()
{
	name = "animator";
}

//Destructor ==========================
j1Animator::~j1Animator()
{

}


//Game Loop ===========================
void j1Animator::Init()
{
	active = false;
	enabled = false;
}

bool j1Animator::Enable()
{
	//Timer to count the operations time and respect the TIME_TO_ENABLE
	j1Timer process_timer;

	//If the enable node is null we focus it to the first one of the data document
	if (current_enable_node.root() == NULL)
	{
		current_enable_node = animations_data.first_child().first_child().first_child();
	}

	//Iterate all the nodes
	while (current_enable_node != NULL)
	{
		//Load the animation data of the current node
		std::string folder = this->name + "/" + current_enable_node.attribute("file").as_string();
		ENTITY_TYPE entity_type = App->entities_manager->StrToEntityType(current_enable_node.attribute("entity_type").as_string());
		
		if (LoadAnimationBlock(folder.c_str(),entity_type))
		{
			LOG("%s correctly loaded!", folder.c_str());
		}
		else LOG("Error loading: %s", folder.c_str());
		
		//Focus the next node
		current_enable_node = current_enable_node.next_sibling();

		//Break the enable process when the time is exceeded
		if (current_enable_node.root() == NULL)
		{
			break;
		}
		
		if(process_timer.Read() > TIME_TO_ENABLE)
		{
			return false;
		}
	}
	
	enabled = true;
	active = false;

	return true;
}

void j1Animator::Disable()
{
	//Clean the data loaded on the animations tree
	uint size = blocks.size();
	for (uint k = 0; k < size; k++)
	{
		blocks[k]->ClearAnimationBlocks();
	}
	
	active = false;
	enabled = false;
	
}

bool j1Animator::Awake(pugi::xml_node& config)
{
	bool ret = true;

	//Load the animations data document from config
	if (App->fs->LoadXML(config.child("animations_data").attribute("file").as_string(), &animations_data))
	{
		LOG("Animations data document loaded!");
		ret = true;
	}
	else
	{
		LOG("Error loading animations data document!");
		ret = false;
	}

	return ret;
}

bool j1Animator::Start()
{
	// arrow = App->tex->Load("buff_manager/arrow.PNG");

	//Define the animations blocks base to build the tree when animations are loaded
	if (blocks.empty())
	{
		//Creatures base animation block
		Animation_Block* creatures_block = new Animation_Block();
		creatures_block->SetId(ENTITY_TYPE::CREATURE);
		blocks.push_back(creatures_block);
		//Items base animation block
		Animation_Block* items_block = new Animation_Block();
		items_block->SetId(ENTITY_TYPE::ITEM);
		blocks.push_back(items_block);

	}
	else
	{
		LOG("Error animations blocks already allocated!");
		return false;
	}

	return true;
}

bool j1Animator::CleanUp()
{
	//Clean the animation blocks
	uint size = blocks.size();

	for (uint k = 0; k < size; k++)
	{
		RELEASE(blocks[k]);
	}
	blocks.clear();

	return true;
}

bool j1Animator::LoadAnimationBlock(const char * xml_folder, ENTITY_TYPE entity_type)
{
	/*//Load animations data from folders
	//XML load
	LOG("Loading: %s", xml_folder);
	std::string load_folder = name + "/" + xml_folder;
	pugi::xml_document entity_anim_data;
	if (!App->fs->LoadXML(load_folder.c_str(), &entity_anim_data))return false;
	//Texture load
	load_folder = name + "/" + entity_anim_data.child("atlas").attribute("texture").as_string();
	SDL_Texture* texture = App->tex->Load(load_folder.c_str());

	//Node to a resource type
	pugi::xml_node entity_node = entity_anim_data.first_child().first_child();
	while (entity_node != NULL)
	{
		//Create a pointer to the new resource AnimationBlock
		Animation_Block* resource_block = new Animation_Block();
		resource_block->SetId(StrToResourceEnum(resource_node.attribute("id").as_string()));

		//Focus the first element of the current resource
		pugi::xml_node element_node = resource_node.first_child();
		while (element_node != NULL)
		{
			//Build an animation block for the current element
			Animation_Block* element_block = new Animation_Block();

			//Build current element animation
			Animation* anim = new Animation();
			anim->SetLoop(false);

			//Focus the first sprite of the current element
			pugi::xml_node sprite = element_node.first_child();
			while (sprite != NULL)
			{
				//Load sprite rect
				SDL_Rect rect = { sprite.attribute("x").as_int(),sprite.attribute("y").as_int(),sprite.attribute("w").as_int(),sprite.attribute("h").as_int() };
				//Load sprite pivot
				float pX = sprite.attribute("pX").as_float() * rect.w;
				pX = (pX > (floor(pX) + 0.5f)) ? ceil(pX) : floor(pX);
				float pY = sprite.attribute("pY").as_float() * rect.h;
				pY = (pY > (floor(pY) + 0.5f)) ? ceil(pY) : floor(pY);
				//Load sprite opacity
				uint opacity = sprite.attribute("opacity").as_uint();
				if (opacity == 0)opacity = 255;

				//Add sprite at animation
				anim->AddSprite(rect, iPoint(pX, pY), sprite.attribute("z").as_int(), opacity);

				//Focus next animation sprite
				sprite = sprite.next_sibling();
			}

			//Set built animation texture
			anim->SetTexture(texture);

			//Set animation of the current element block
			element_block->SetAnimation(anim);

			//Add element block to the resource childs list
			resource_block->AddAnimationBlock(element_block);

			//Focus the next element node
			element_node = element_node.next_sibling();
		}

		//Add resource block built at the resources vector
		resource_blocks.push_back(resource_block);

		resource_node = resource_node.next_sibling();
	}*/

	return true;
}

bool j1Animator::CreaturePlay(Creature* target)
{
	/*DIRECTION_TYPE direction = target->GetDirection();
	switch (direction)
	{
	case NORTH:	target->SetFlipSprite(false);	break;

	case NORTH_EAST:
		direction = NORTH_WEST;
		target->SetFlipSprite(true);
		break;

	case EAST:
		direction = WEST;
		target->SetFlipSprite(true);
		break;

	case SOUTH_EAST:
		direction = SOUTH_WEST;
		target->SetFlipSprite(true);
		break;

	case SOUTH:			target->SetFlipSprite(false);	break;
	case SOUTH_WEST:	target->SetFlipSprite(false);	break;
	case WEST:			target->SetFlipSprite(false);	break;
	case NORTH_WEST:	target->SetFlipSprite(false);	break;

	}
	DiplomaticAnimation_Block* block = nullptr;

	//Iterate all blocks of childs vector
	uint size = unit_blocks.size();
	for (uint k = 0; k < size; k++)
	{
		//Pointer to the current block
		block = unit_blocks[k];

		//Compare block unit id
		if (block->GetId() == target->GetUnitType())
		{
			if (block->GetId() == VILLAGER)
			{
				//If the unit is a villager we need to check the item that is carrying
				block = block->SearchId(((Villager*)target)->GetItemType());
			}
			//Compare block action id
			block = block->SearchId(target->GetAction());
			//If action block is found search the correct direction block
			if (block != nullptr)block = block->SearchId(direction);
			//If direction block is found returns the block animation
			if (block != nullptr)
			{
				target->CleanAnimation();
				DiplomaticAnimation* anim = new DiplomaticAnimation(*block->GetAnimation());
				if (anim != nullptr)
				{
					target->SetAnimation((Animation*)anim);
					target->GetAnimation()->Reset();
				}
				else LOG("Error in unit animation Play");

				return true;
			}
		}
	}*/

	return false;
}

bool j1Animator::ItemPlay(Item* target)
{
	/*DiplomaticAnimation_Block* block = nullptr;

	//Iterate all blocks of childs vector
	uint size = building_blocks.size();
	for (uint k = 0; k < size; k++)
	{
		//Pointer to the current block
		block = building_blocks[k];

		//Compare block unit id
		if (block->GetId() == target->GetBuildingType())
		{
			//Compare block action id
			block = block->SearchId(target->GetActionType());
			//If action block is found search the correct direction block or return unidirectional action
			if (target->GetDirectionType() == NO_DIRECTION)
			{
				DiplomaticAnimation* anim = new DiplomaticAnimation(*block->GetAnimation());
				if (anim == nullptr)LOG("Error in building Play");
				target->CleanAnimation();
				target->SetAnimation((Animation*)anim);
				return true;
			}
			if (block != nullptr)block = block->SearchId(target->GetDirectionType());
			//If direction block is found returns the block animation
			if (block != nullptr)
			{
				DiplomaticAnimation* anim = new DiplomaticAnimation(*block->GetAnimation());
				if (anim == nullptr)LOG("Error in building Play");
				target->CleanAnimation();
				target->SetAnimation((Animation*)anim);
				return true;
			}
		}
	}*/

	return false;
}
