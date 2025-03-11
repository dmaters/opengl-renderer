#pragma once

#include "Texture.h"

class PostProcessingEffect {
protected:
	PostProcessingEffect() {}

public:
	virtual void run(Texture& input, Texture& output) = 0;
};
