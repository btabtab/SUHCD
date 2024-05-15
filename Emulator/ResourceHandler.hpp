#include <raylib.h>
#include <vector>
class ResourceHandler
{
private:
	std::vector<Texture> textures;
	std::vector<Sound> sounds;

public:
	ResourceHandler();

	void addTexture(Texture new_texture)
	{
		textures.push_back(new_texture);
	}
	void addSound(Sound new_sound)
	{
		sounds.push_back(new_sound);
	}

	Texture getTextureAtIndex(int index)
	{
		return textures.at(index);
	}
	Sound getSoundAtIndex(int index)
	{
		return sounds.at(index);
	}
};