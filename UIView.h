#pragma once
#include <vector>
namespace KEngine2D
{
	class HierarchicalTransform;
}

namespace KEngineBasics {
	
	
	enum UIDimension
	{
		Width,
		Height
	};

	enum UISide
	{
		Left,
		Top,
		Right,
		Bottom
	};

	class UIView;
	class UIViewFactory;

	class UILayoutGuide
	{
	public:
		UILayoutGuide() {};
		virtual ~UILayoutGuide() {};
		virtual float GetValue(const UIView* forView) const = 0;
	};

	class UIStaticLayoutGuide : public UILayoutGuide
	{
	public:
		UIStaticLayoutGuide();
		virtual ~UIStaticLayoutGuide();

		void Init(float value);
		void Deinit();

		virtual float GetValue(const UIView *forView) const override;
	private:
		float mValue;
	};

	class UIParentRelativeLayoutGuide : public UILayoutGuide
	{
	public:
		UIParentRelativeLayoutGuide();
		virtual ~UIParentRelativeLayoutGuide();

		void Init(UIDimension dimension, float ratio, float offset);
		void Deinit();

		virtual float GetValue(const UIView* forView) const override;
	private:
		UIDimension mDimension;
		float		mRatio;
		float		mOffset;
	};

	class UISiblingRelativeLayoutGuide : public UILayoutGuide
	{
	public:
		UISiblingRelativeLayoutGuide();
		virtual ~UISiblingRelativeLayoutGuide();

		void Init(const UILayoutGuide* backup, UIDimension dimension, float ratio, float offset);
		void Deinit();
		virtual float GetValue(const UIView* forView) const override;
	private:
		UIDimension				mDimension{ Width };
		float					mRatio{ 1.0 };
		float					mOffset{ 0.0 };
		const UILayoutGuide*	mBackup{ nullptr };
	};

	class UIContentRelativeLayoutGuide : public UILayoutGuide
	{
	public:
		UIContentRelativeLayoutGuide();
		virtual ~UIContentRelativeLayoutGuide();

		void Init(const UILayoutGuide * relativeTo, UIDimension dimension, float ratio, float offset);
		void Deinit();

		virtual float GetValue(const UIView* forView) const override;
	private:	
		const UILayoutGuide*	mRelativeTo{ nullptr };
		UIDimension				mDimension{ Width };
		float					mRatio{ 1.0 };
		float					mOffset{ 0.0 };
	};

	class UIView
	{
	public:
		UIView();
		virtual ~UIView();

		void Init(const UILayoutGuide* left, const UILayoutGuide* top, const UILayoutGuide* right, const UILayoutGuide* bottom, int layer, KEngine2D::HierarchicalTransform* transform, UIView * parent);
		void Deinit();

		float GetLeft() const;
		float GetTop() const;
		float GetRight() const;
		float GetBottom() const;

		virtual float GetContentSize(UIDimension d) const;

		const UIView* GetParent() const;
		const UIView* GetOlderSibling() const;
		int GetNumChildren() const;
		const UIView* GetChild(int index) const;

		virtual int GetChildLayer() const;

		void AddChild(UIView* child);
		void RemoveChild(UIView* child);

		void UpdateLayout(bool parentFirst = true);

		KEngine2D::HierarchicalTransform* GetTransform() const;
	protected:
		const UILayoutGuide*	mLeft{ nullptr };
		const UILayoutGuide*	mTop{ nullptr };
		const UILayoutGuide*	mRight{ nullptr };
		const UILayoutGuide*	mBottom{ nullptr };
		std::vector<UIView*>	mChildren;
		UIView*					mParent{ nullptr };
		const UIView*			mSibling{ nullptr };
		int						mLayer;

		KEngine2D::HierarchicalTransform* mTransform;
	};




	/*
	
	Rework ideas

	Separate concepts - 

	Abstract View - data source
	View Layout - left, top, right, bottom
	View Hierarchy - parent, children
	Transform - updated by Layout + Hierarchy
	TextSprite - updated by Abstract View
	Spite - update by Abstract View

	
	A "UI" is an abstract view + a Static Transform + a RecyclableGroup

	EVERY stage of view hierarchy has hierarchical transform

	UNLESS we need to do tweens.

	Should tweens function on hierarchical transforms? YES.

	Therefore every stage is hierarchical.  


	
	
	
	
	
	
	
	
	
	*/




}