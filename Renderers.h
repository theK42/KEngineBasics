#pragma once

#if defined(KENGINE_FLEX_RENDER)
	static_assert(false, "Flex rendering not yet implemented");
#elif defined(KENGINE_USE_OPENGL)
#include "../KEngineOpenGL/SpriteRenderer.h"
#include "../KEngineOpenGL/TextRenderer.h"
#include "../KEngineOpenGL/FontFactory.h"
#include "../KEngineOpenGL/ShaderFactory.h"
#include "../KEngineOpenGL/TextureFactory.h"

namespace KEngineGraphics = KEngineOpenGL;

/*namespace KEngineBasics
{
class SpriteRenderer : public KEngineOpenGL::SpriteRenderer{};
class Sprite : public KEngineOpenGL::Sprite{};
class SpriteGraphic : public KEngineOpenGL::SpriteGraphic{};
class TextRenderer : public KEngineOpenGL::TextRenderer{};
class TextSprite : public KEngineOpenGL::TextSprite{};
class FontFactory : public KEngineOpenGL::FontFactory{};
class ShaderFactory : public KEngineOpenGL::ShaderFactory{};
//typedef KEngineOpenGL::SpriteRenderer SpriteRenderer;
	//typedef KEngineOpenGL::Sprite Sprite;
	//typedef KEngineOpenGL::SpriteGraphic SpriteGraphic;
	//typedef KEngineOpenGL::TextRenderer TextRenderer;
	//typedef KEngineOpenGL::TextSprite TextSprite;


//	typedef KEngineOpenGL::FontFactory FontFactory;
//	typedef KEngineOpenGL::ShaderFactory ShaderFactory;
}*/

#elif defined(KENGINE_USE_VULKAN)
    static_assert(false, "Vulkan rendering not yet implmented");
#elif defined(KENGINE_USE_METAL)
	static_assert(false, "Metal rendering not yet implemented");
#endif
