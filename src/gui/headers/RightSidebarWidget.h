#pragma once

#include "../headers/Widget.h"

class RightSidebarWidget : Widget
{
private:
	bool popupOpen;
	unsigned int selectedTexture;
	void TextureModal(const ImTextureID& id);

public:
	RightSidebarWidget();

	void layersControl();
	void textureView();
	void environmentControl();
	void render() override;
};

