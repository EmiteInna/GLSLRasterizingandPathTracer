#ifndef EMITEINNA_TEXTURE
#define EMITEINNA_TEXTURE

#include "core.h"
class Texture {
public:
	unsigned int ID;
	int GetTextureID();
	Texture(const std::string& path);
};


#endif