#pragma once
#include "UIView.h"
#include "UITextView.h"

#include "Renderers.h"
#include "Disposable.h"
#include "HierarchicalTransform2D.h"



namespace KEngineBasics {		
		
	class UIViewFactory
	{
	public:
		UIViewFactory();
		~UIViewFactory();
        void Init(KEngineGraphics::ShaderFactory* shaderFactory, KEngineGraphics::FontFactory* fontFactory, KEngineGraphics::SpriteRenderer* renderer, KEngineGraphics::TextRenderer* textRenderer, KEngine2D::HierarchyUpdater* hierarchySystem, int uiLayer);
		void Deinit();

		KEngineCore::DisposableGroup* CreateUIGroup();

		UILayoutGuide* CreateStaticLayoutGuide(KEngineCore::DisposableGroup* disposables, float value);
		UILayoutGuide* CreateParentRelativeLayoutGuide(KEngineCore::DisposableGroup* disposables, UIDimension dimension, float ratio, float offset);

		UILayoutGuide* CreateSiblingRelativeLayoutGuide(KEngineCore::DisposableGroup* disposables, UILayoutGuide* backup, UIDimension dimension, float ratio, float offset);
		UILayoutGuide* CreateContentRelativeLayoutGuide(KEngineCore::DisposableGroup* disposables, UILayoutGuide* relativeTo,  UIDimension dimension, float ratio, float offset);


		KEngine2D::HierarchicalTransform* CreateTransform(KEngineBasics::UIView* parent, KEngineCore::DisposableGroup* disposables);

		UIView* CreateUIView(KEngineCore::DisposableGroup* disposables, UIView* parent, UILayoutGuide* left, UILayoutGuide* top, UILayoutGuide* right, UILayoutGuide* bottom);
		UITextView* CreateUITextView(KEngineCore::DisposableGroup* disposables, UIView* parent, UILayoutGuide* left, UILayoutGuide* top, UILayoutGuide* right, UILayoutGuide* bottom, KEngineCore::StringHash fontName, KEngineCore::StringHash shaderName, std::string_view initialText);


	private:
			
        KEngineGraphics::ShaderFactory*	mShaderFactory{ nullptr };
        KEngineGraphics::FontFactory*	mFontFactory{ nullptr };

        KEngineGraphics::SpriteRenderer*					mRenderer{ nullptr };
        KEngineGraphics::TextRenderer*					mTextRenderer{ nullptr };
		KEngine2D::HierarchyUpdater*	mHierarchySystem{ nullptr };

		KEngineCore::RecyclingPool<KEngineGraphics::Sprite>			mSpritePool;
		KEngineCore::RecyclingPool<KEngineGraphics::TextSprite>		mTextPool;
		KEngineCore::RecyclingPool<KEngineGraphics::SpriteGraphic>	mGraphicPool;

		KEngineCore::RecyclingPool<KEngine2D::StaticTransform>					mStaticTransformPool;
		KEngineCore::RecyclingPool<KEngine2D::UpdatingHierarchicalTransform>	mHierarchicalTransformPool;

		KEngineCore::RecyclingPool<UIStaticLayoutGuide>				mStaticLayoutPool;
		KEngineCore::RecyclingPool<UIParentRelativeLayoutGuide>		mParentRelativeLayoutPool;
		KEngineCore::RecyclingPool<UISiblingRelativeLayoutGuide>	mSiblingRelativeLayoutPool;
		KEngineCore::RecyclingPool<UIContentRelativeLayoutGuide>	mContentRelativeLayoutPool;

		KEngineCore::RecyclingPool<UIView>		mViewPool;
		KEngineCore::RecyclingPool<UITextView>	mTextViewPool;

		
		KEngineCore::Pool<KEngineCore::DisposableGroup>	mRootPool;
		
		int mDefaultLayer;
	};
}

