#pragma once

#if defined(KENGINE_FLEX_RENDER)
	static_assert(false, "Flex rendering not yet implemented");
#elif defined(KENGINE_USE_OPENGL)
#include "../KEngineOpenGL/SpriteRenderer.h"
#include "../KEngineOpenGL/TextRenderer.h"
#include "../KEngineOpenGL/FontFactory.h"
#include "../KEngineOpenGL/ShaderFactory.h"

namespace KEngineBasics
{
	typedef KEngineOpenGL::SpriteRenderer SpriteRenderer;
	typedef KEngineOpenGL::Sprite Sprite;
	typedef KEngineOpenGL::SpriteGraphic SpriteGraphic;
	typedef KEngineOpenGL::TextRenderer TextRenderer;
	typedef KEngineOpenGL::TextSprite TextSprite;


	typedef KEngineOpenGL::FontFactory FontFactory;
	typedef KEngineOpenGL::ShaderFactory ShaderFactory;
}

#elif defined(KENGINE_USE_VULKAN)

#elif defined(KENGINE_USE_METAL)
	static_assert(false, "Metal rendering not yet implemented");
#endif
