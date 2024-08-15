#pragma once

#include "../headers/Widget.h"

class RightSidebarWidget : Widget
{
private:
	const std::string ACTIVE_SCENE = "default";
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

