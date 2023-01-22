#include "UIViewFactory.h"

KEngineBasics::UIViewFactory::UIViewFactory()
{
}

KEngineBasics::UIViewFactory::~UIViewFactory()
{
	Deinit();
}

void KEngineBasics::UIViewFactory::Init(ShaderFactory* shaderFactory, FontFactory* fontFactory, SpriteRenderer* renderer, TextRenderer* textRenderer, KEngine2D::HierarchyUpdater* hierarchySystem, int uiLayer)
{
	assert(mRenderer == nullptr);
	mShaderFactory = shaderFactory;
	mFontFactory = fontFactory;
	mRenderer = renderer;
	mTextRenderer = textRenderer;
	mHierarchySystem = hierarchySystem;
	mSpritePool.Init();
	mTextPool.Init();
	mGraphicPool.Init();
	mStaticLayoutPool.Init();
	mParentRelativeLayoutPool.Init();
	mSiblingRelativeLayoutPool.Init();
	mContentRelativeLayoutPool.Init();
	mViewPool.Init();
	mTextViewPool.Init();
	mStaticTransformPool.Init();
	mHierarchicalTransformPool.Init();
	mRootPool.Init();
	mDefaultLayer = uiLayer;
}

void KEngineBasics::UIViewFactory::Deinit()
{
	mShaderFactory = nullptr;
	mFontFactory = nullptr;
	mRenderer = nullptr;
	mTextRenderer = nullptr;
	mHierarchySystem = nullptr;
	mSpritePool.Deinit();
	mTextPool.Deinit();
	mGraphicPool.Deinit();
	mStaticLayoutPool.Deinit();
	mParentRelativeLayoutPool.Deinit();
	mSiblingRelativeLayoutPool.Deinit();
	mContentRelativeLayoutPool.Deinit();
	mViewPool.Deinit();
	mTextViewPool.Deinit();
	mStaticTransformPool.Deinit();
	mHierarchicalTransformPool.Deinit();
	mRootPool.Deinit();
}

KEngineCore::DisposableGroup* KEngineBasics::UIViewFactory::CreateUIGroup()
{
	return mRootPool.GetItem();
}

KEngineBasics::UILayoutGuide* KEngineBasics::UIViewFactory::CreateStaticLayoutGuide(KEngineCore::DisposableGroup* disposables, float value)
{
	auto guide = mStaticLayoutPool.GetItem(disposables);
	guide->Init(value);
	return guide;
}

KEngineBasics::UILayoutGuide* KEngineBasics::UIViewFactory::CreateParentRelativeLayoutGuide(KEngineCore::DisposableGroup* disposables, UIDimension dimension, float ratio, float offset)
{
	auto guide = mParentRelativeLayoutPool.GetItem(disposables);
	guide->Init(dimension, ratio, offset);
	return guide;
}

KEngineBasics::UILayoutGuide* KEngineBasics::UIViewFactory::CreateSiblingRelativeLayoutGuide(KEngineCore::DisposableGroup* disposables, UILayoutGuide* backup, UIDimension dimension, float ratio, float offset)
{
	auto guide = mSiblingRelativeLayoutPool.GetItem(disposables);
	guide->Init(backup, dimension, ratio, offset);
	return guide;
}

KEngineBasics::UILayoutGuide* KEngineBasics::UIViewFactory::CreateContentRelativeLayoutGuide(KEngineCore::DisposableGroup* disposables, UILayoutGuide* relativeTo, UIDimension dimension, float ratio, float offset)
{
	auto guide = mContentRelativeLayoutPool.GetItem(disposables);
	guide->Init(relativeTo, dimension, ratio, offset);
	return guide;
}

KEngine2D::HierarchicalTransform * KEngineBasics::UIViewFactory::CreateTransform(KEngineBasics::UIView* parent, KEngineCore::DisposableGroup* disposables)
{
	auto transform = mHierarchicalTransformPool.GetItem(disposables);
	if (!parent)
	{
		auto rootTransform = mStaticTransformPool.GetItem(disposables);
		rootTransform->Init();
		transform->Init(mHierarchySystem, rootTransform);
	}
	else
	{
		transform->Init(mHierarchySystem, parent->GetTransform());
	}
	return transform;
}

KEngineBasics::UIView* KEngineBasics::UIViewFactory::CreateUIView(KEngineCore::DisposableGroup* disposables, UIView* parent, UILayoutGuide* left, UILayoutGuide* top, UILayoutGuide* right, UILayoutGuide* bottom)
{
	auto view = mViewPool.GetItem(disposables);
	auto transform = CreateTransform(parent, disposables);
	int layer = mDefaultLayer;
	if (parent != nullptr)
	{
		layer = parent->GetChildLayer();
	}

	view->Init(left, top, right, bottom, layer, transform, parent);
	return view;
}

KEngineBasics::UITextView* KEngineBasics::UIViewFactory::CreateUITextView(KEngineCore::DisposableGroup* disposables, UIView* parent, UILayoutGuide* left, UILayoutGuide* top, UILayoutGuide* right, UILayoutGuide* bottom, KEngineCore::StringHash fontName, KEngineCore::StringHash shaderName, std::string_view initialText)
{
	auto textView = mTextViewPool.GetItem(disposables);
	auto transform = CreateTransform(parent, disposables);
	int layer = mDefaultLayer;
	if (parent != nullptr)
	{
		layer = parent->GetChildLayer();
	}
	textView->Init(mFontFactory->GetFont(fontName), mShaderFactory->GetShaderProgram(shaderName), mRenderer, mTextRenderer, initialText, left, top, right, bottom, layer, transform, parent);
	return textView;
}
