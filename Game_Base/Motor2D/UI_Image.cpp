#include "UI_Image.h"
#include "j1Render.h"

//Constructors ============================================
UI_Image::UI_Image(const SDL_Rect& box, const iPoint pivot, const SDL_Rect& texture_rect, TEXTURE_ID texture_id) : UI_Element(box, IMG), pivot(pivot), texture_rect(texture_rect), texture_id(texture_id)
{
	texture = App->gui->Get_UI_Texture(texture_id);
}

UI_Image::UI_Image(const UI_Image* copy) : UI_Element(copy->box, IMG), pivot(copy->pivot), texture_rect(copy->texture_rect), texture_scale(copy->texture_scale), texture_id(copy->texture_id) {}

UI_Image::UI_Image() : UI_Element({ 0,0,0,0 }, IMG), pivot(0,0), texture_rect({ 0,0,0,0 }) {}

//Destructors =============================================
UI_Image::~UI_Image()
{

}


//Game Loop ===============================================
void UI_Image::Draw(bool debug) const
{
	//This Draw
	if (debug)
	{
		App->render->DrawQuad({ box.x - App->render->camera.x, box.y - App->render->camera.y, box.w, box.h }, 150, 50, 0);
	}

	//Draw from other textures
	else {
		
		//Undefined draw size
		if (texture_rect.w == 0 || texture_rect.h == 0)App->render->CallBlit(texture, box.x - App->render->camera.x - pivot.x, box.y - App->render->camera.y - pivot.y, NULL, false, texture_scale, visual_layer);
		//Defined draw size
		else App->render->CallBlit(texture, box.x - App->render->camera.x - pivot.x, box.y - App->render->camera.y - pivot.y, &texture_rect, false, texture_scale, visual_layer);

	}

	//Childs Draw
	DrawChilds(debug);
}

//Functionality ===========================================
SDL_Rect UI_Image::AdjustBox()
{
	
	box.w = texture_rect.w;
	box.h = texture_rect.h;

	if (box.w == 0 || box.h == 0) {
	
		int w, h;
		SDL_QueryTexture(texture, NULL, NULL, &w, &h);

		box.w = w * texture_scale;
		box.h = h * texture_scale;
	}

	return box;
}

void UI_Image::DrawAt(int x, int y) const
{
	x += this->box.x;
	y += this->box.y;

	//Undefined draw size
	if (texture_rect.w == 0 || texture_rect.h == 0)App->render->CallBlit(texture, x - App->render->camera.x - pivot.x, y - App->render->camera.y - pivot.y, NULL, false, texture_scale, visual_layer);
	//Defined draw size
	else App->render->CallBlit(texture, x - App->render->camera.x - pivot.x, y - App->render->camera.y - pivot.y, &texture_rect, false, texture_scale, visual_layer);
		
}

void UI_Image::ChangeTextureRect(SDL_Rect new_rect)
{
	texture_rect = new_rect;
}

void UI_Image::ChangeTextureId(TEXTURE_ID texture_id)
{
	this->texture_id = texture_id;
	texture = App->gui->Get_UI_Texture(texture_id);
}

iPoint UI_Image::GetPivot() const
{
	return pivot;
}

SDL_Rect UI_Image::GetTextureBox() const
{
	return texture_rect;
}

float UI_Image::GetTextureScale() const
{
	return texture_scale;
}

void UI_Image::SetTextureScale(float val)
{
	texture_scale = val;
}
