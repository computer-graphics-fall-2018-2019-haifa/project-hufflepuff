#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include <glad/glad.h>
#include <string>
#include <algorithm>

using std::string;

struct Vec {
	int x, y;
	float r, g, b;
};


class Texture2D
{
public:
	Texture2D();
	virtual ~Texture2D();
	Texture2D(const Texture2D& rhs) {}
	Texture2D& operator = (const Texture2D& rhs) {}

	bool loadTexture(const string& fileName, bool generateMipMaps = true);
	void bind(GLuint texUnit = 0) const;
	void unbind(GLuint texUnit = 0) const;

	bool generateBombingTexture(bool generateMipMaps);

private:
	GLuint mTexture;
};
#endif
