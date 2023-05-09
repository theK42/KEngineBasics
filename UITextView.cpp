#include "UITextView.h"
#include "HierarchicalTransform2D.h"

KEngineBasics::UITextView::UITextView()
{
}

KEngineBasics::UITextView::~UITextView()
{
	Deinit();
}

void KEngineBasics::UITextView::Init(const KEngineOpenGL::FixWidthBitmapFont& font, const KEngineOpenGL::ShaderProgram* shaderProgram, KEngineOpenGL::SpriteRenderer* renderer, KEngineOpenGL::TextRenderer* textRenderer, std::string_view text, const UILayoutGuide* left, const UILayoutGuide* top, const UILayoutGuide* right, const UILayoutGuide* bottom, int layer, KEngine2D::HierarchicalTransform * transform, UIView* parent)
{
	mFont = &font;
	mSprite.Init(0, 0, shaderProgram, textRenderer);
	UIView::Init(left, top, right, bottom, layer, transform, parent);
	mGraphic.Init(renderer, &mSprite.GetSprite(), mTransform, layer);	
	SetText(text);
}

void KEngineBasics::UITextView::Deinit()
{
	mText = "";
	mGraphic.Deinit();
	mSprite.Deinit();
	UIView::Deinit();
}

void KEngineBasics::UITextView::SetText(std::string_view text)
{
	if (mText != text) {
		mText = text;
		float width = mRight->GetValue(this) - mLeft->GetValue(this);
		float height = mBottom->GetValue(this) - mTop->GetValue(this);

		if (mSprite.GetSprite().width != width || mSprite.GetSprite().height != height)
		{
			mSprite.Resize(width, height);

			UpdateLayout();
		}

		mSprite.RenderText(mText, *mFont);
	}
}

int KEngineBasics::UITextView::GetChildLayer() const
{
	return mLayer + 1;
}

float KEngineBasics::UITextView::GetContentSize(UIDimension d) const
{
	float selfSize = 0.0f;
	switch (d)
	{
	case Width:
		selfSize = mSprite.GetRenderer()->GetTextWidth(mText, *mFont);
		break;
	case Height:
		selfSize = mSprite.GetRenderer()->GetTextHeight(mText, *mFont);
		break;
	}

	return std::max(selfSize, UIView::GetContentSize(d));
}
