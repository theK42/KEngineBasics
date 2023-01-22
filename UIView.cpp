#include "UIView.h"
#include "UIViewFactory.h"
#include <assert.h>

KEngineBasics::UIStaticLayoutGuide::UIStaticLayoutGuide()
{
}

KEngineBasics::UIStaticLayoutGuide::~UIStaticLayoutGuide()
{
	Deinit();
}

void KEngineBasics::UIStaticLayoutGuide::Init(float value)
{
	mValue = value;
}

void KEngineBasics::UIStaticLayoutGuide::Deinit()
{
}

float KEngineBasics::UIStaticLayoutGuide::GetValue(const UIView* forView) const
{
	return mValue;
}

KEngineBasics::UIParentRelativeLayoutGuide::UIParentRelativeLayoutGuide()
{
}

KEngineBasics::UIParentRelativeLayoutGuide::~UIParentRelativeLayoutGuide()
{
	Deinit();
}

void KEngineBasics::UIParentRelativeLayoutGuide::Init(UIDimension dimension, float ratio, float offset)
{
	mDimension = dimension;
	mRatio = ratio;
	mOffset = offset;
}

void KEngineBasics::UIParentRelativeLayoutGuide::Deinit()
{
}

float KEngineBasics::UIParentRelativeLayoutGuide::GetValue(const UIView* forView) const
{	
	float begin;
	float end;
	const UIView* parent = forView->GetParent();
	assert(parent != nullptr);
	switch (mDimension)
	{
	case Width:
		begin = parent->GetLeft();
		end = parent->GetRight();
		break;
	case Height:
		begin = parent->GetTop();
		end = parent->GetBottom();
		break;
	default:
		begin = 0;
		end = 0;
		assert(false);
	}
	return (mRatio * (end - begin)) + begin + mOffset;
}

KEngineBasics::UIContentRelativeLayoutGuide::UIContentRelativeLayoutGuide()
{
}

KEngineBasics::UIContentRelativeLayoutGuide::~UIContentRelativeLayoutGuide()
{
	Deinit();
}

void KEngineBasics::UIContentRelativeLayoutGuide::Init(const UILayoutGuide * relativeTo, UIDimension dimension, float ratio, float offset)
{
	assert(mRelativeTo == nullptr);
	mRelativeTo = relativeTo;
	mDimension = dimension;
	mRatio = ratio;
	mOffset = offset;
}

void KEngineBasics::UIContentRelativeLayoutGuide::Deinit()
{
	mRelativeTo = nullptr;
}

float KEngineBasics::UIContentRelativeLayoutGuide::GetValue(const UIView* forView) const
{
	float relativePoint = mRelativeTo->GetValue(forView);
	float contentSize = forView->GetContentSize(mDimension);
	return relativePoint + (contentSize * mRatio) + mOffset;
}

KEngineBasics::UIView::UIView()
{
}

KEngineBasics::UIView::~UIView()
{
	Deinit();
}

void KEngineBasics::UIView::Init(const UILayoutGuide* left, const UILayoutGuide* top, const UILayoutGuide* right, const UILayoutGuide* bottom, int layer, KEngine2D::HierarchicalTransform * transform,  UIView* parent)
{
	assert(mLeft == nullptr);
	mLeft = left;
	mTop = top;
	mRight = right;
	mBottom = bottom;
	mParent = parent;
	if (parent != nullptr)
	{
		if (parent->GetNumChildren() > 0) {
			mSibling = parent->GetChild(parent->GetNumChildren() - 1);
		}
		parent->AddChild(this);
	}
	mTransform = transform;
	UpdateLayout();
	mLayer = layer;
}

void KEngineBasics::UIView::Deinit()
{
	for (auto child : mChildren)
	{
		child->Deinit();
	}
	if (mParent != nullptr)
	{
		mParent->RemoveChild(this);
	}
	mTransform = nullptr;
	mLeft = nullptr;
	mTop = nullptr;
	mRight = nullptr;
	mBottom = nullptr;
	mParent = nullptr;
}

float KEngineBasics::UIView::GetLeft() const
{
	return mLeft->GetValue(this);
}

float KEngineBasics::UIView::GetTop() const
{
	return mTop->GetValue(this);
}

float KEngineBasics::UIView::GetRight() const
{
	return mRight->GetValue(this);
}

float KEngineBasics::UIView::GetBottom() const
{
	return mBottom->GetValue(this);
}

float KEngineBasics::UIView::GetContentSize(UIDimension d) const
{
	if (mChildren.size() == 0)
	{
		return 0.0f;
	}
	float min = std::numeric_limits<float>::max();
	float max = std::numeric_limits<float>::lowest();
	for (auto child : mChildren)
	{
		switch (d)
		{
		case Width:
			min = std::min(min, child->GetLeft());
			max = std::max(max, child->GetRight());
			break;
		case Height:
			min = std::min(min, child->GetTop());
			max = std::max(max, child->GetBottom());
			break;
		default:
			assert(false);
		}
	}
	return max - min;
}

const KEngineBasics::UIView* KEngineBasics::UIView::GetParent() const
{
	return mParent;
}

const KEngineBasics::UIView* KEngineBasics::UIView::GetOlderSibling() const
{
	return mSibling;
}

int KEngineBasics::UIView::GetNumChildren() const
{
	return mChildren.size();
}

const KEngineBasics::UIView* KEngineBasics::UIView::GetChild(int index) const
{
	return mChildren[index];
}

int KEngineBasics::UIView::GetChildLayer() const
{
	return mLayer;
}

void KEngineBasics::UIView::AddChild(UIView* child)
{
	mChildren.push_back(child);
}

void KEngineBasics::UIView::RemoveChild(UIView* child)
{
	mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), child), mChildren.end());
}

//TODO:  Mark dirty, update once per frame.  
void KEngineBasics::UIView::UpdateLayout(bool parentFirst)
{
	if (parentFirst && mParent != nullptr)
	{
		mParent->UpdateLayout(parentFirst);
	}
	else
	{
		KEngine2D::StaticTransform temp;

		float left = GetLeft();
		float top = GetTop();

		if (mParent != nullptr)
		{
			left -= mParent->GetLeft();
			top -= mParent->GetTop();
		}

 		temp.SetTranslation({left, top});
		mTransform->SetLocalTransform(temp);
		for (auto child : mChildren)
		{
			child->UpdateLayout(false);
		}
	}
}

KEngine2D::HierarchicalTransform* KEngineBasics::UIView::GetTransform() const
{
	return mTransform;
}

KEngineBasics::UISiblingRelativeLayoutGuide::UISiblingRelativeLayoutGuide()
{
}

KEngineBasics::UISiblingRelativeLayoutGuide::~UISiblingRelativeLayoutGuide()
{
	Deinit();
}

void KEngineBasics::UISiblingRelativeLayoutGuide::Init(const UILayoutGuide* backup, UIDimension dimension, float ratio, float offset)
{
	assert(mBackup == nullptr);
	mDimension = dimension;
	mRatio = ratio;
	mOffset = offset;
	mBackup = backup;
}

void KEngineBasics::UISiblingRelativeLayoutGuide::Deinit()
{
	mBackup = nullptr;
}

float KEngineBasics::UISiblingRelativeLayoutGuide::GetValue(const UIView* forView) const
{
	float begin;
	float end;
	const UIView* parent = forView->GetParent();
	assert(parent != nullptr);
	const UIView* sibling = forView->GetOlderSibling();
	if (sibling == nullptr)
	{
		return mBackup->GetValue(forView);
	}
	switch (mDimension)
	{
	case Width:
		begin = sibling->GetLeft();
		end = sibling->GetRight();
		break;
	case Height:
		begin = sibling->GetTop();
		end = sibling->GetBottom();
		break;
	default:
		begin = 0;
		end = 0;
		assert(false);
	}
	return (mRatio * (end - begin)) + begin + mOffset;
}
