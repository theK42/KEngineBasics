#pragma once
#include "UIView.h"
#include "Renderers.h"
#include <string_view>

namespace KEngineOpenGL
{
	struct FixWidthBitmapFont;
	struct ShaderProgram;
	class TextRenderer;
	class TextSprite;
	class SpriteRenderer;
	class SpriteGraphic;
}

namespace KEngineBasics {
	class UITextView : public UIView
	{
	public:
		UITextView();
		virtual ~UITextView();

		void Init(const KEngineOpenGL::FixWidthBitmapFont& font, const KEngineOpenGL::ShaderProgram* shaderProgram, KEngineOpenGL::SpriteRenderer* renderer, KEngineOpenGL::TextRenderer* textRenderer, std::string_view text, const UILayoutGuide* left, const UILayoutGuide* top, const UILayoutGuide* right, const UILayoutGuide* bottom, int layer, KEngine2D::HierarchicalTransform* transform, UIView* parent);
		void Deinit();

		void SetText(std::string_view text);
		int GetChildLayer() const override;
		
		float GetContentSize(UIDimension d) const override;

	private:
		std::string									mText;
		const KEngineOpenGL::FixWidthBitmapFont*	mFont { nullptr };

		KEngineOpenGL::TextSprite		mSprite;
		KEngineOpenGL::SpriteGraphic	mGraphic; 
		
	};
}
