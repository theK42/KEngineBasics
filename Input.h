#pragma once

#include "Transform2D.h"
#include "StringHash.h"
#include "LuaLibrary.h"
#include "Timer.h"
#include <set>
#include <map>
#include <vector>
#include <list>


namespace KEngineCore
{
	class LuaScheduler;
}


namespace KEngineBasics {

	class CombinedAxisBinding;
	class AxisBinding;
	class ButtonDownBinding;
	class ButtonUpBinding;
	class ButtonHoldBinding;
	class Input;


	enum ControllerType {
		Keyboard,
		Gamepad,
		Joystick,
		Virtual
	};

	enum AxisType {
		Horizontal,
		Vertical
	};

	class ButtonDownBinding
	{
	public:
		ButtonDownBinding();
		~ButtonDownBinding();
		void Init(Input* inputSystem, KEngineCore::StringHash buttonName, std::function<void()> callback, std::function<void()> cancelCallback = nullptr);
		void Deinit();

		typedef std::list<ButtonDownBinding*>::iterator Position;
		void SetPosition(Position position);
		Position GetPosition();

		KEngineCore::StringHash GetButtonName() const;

		void Fire();
		void Cancel();
	private:
		Input*					mInputSystem{ nullptr };
		KEngineCore::StringHash mButtonName;
		Position				mPosition;
		std::function<void()>	mCallback;
		std::function<void()>	mCancelCallback;
	};

	class ButtonUpBinding
	{
	public:
		ButtonUpBinding();
		~ButtonUpBinding();
		void Init(Input* inputSystem, KEngineCore::StringHash buttonName, std::function<void()> callback, std::function<void()> cancelCallback = nullptr);
		void Deinit();

		typedef std::list<ButtonUpBinding*>::iterator Position;
		void SetPosition(Position position);
		Position GetPosition();

		KEngineCore::StringHash GetButtonName() const;

		void Fire();
		void Cancel();
	private:
		Input*					mInputSystem{ nullptr };
		KEngineCore::StringHash	mButtonName;
		Position				mPosition;
		std::function<void()>	mCallback;
		std::function<void()>	mCancelCallback;
	};

	class ButtonHoldBinding
	{
	public:
		ButtonHoldBinding();
		~ButtonHoldBinding();
		void Init(Input* inputSystem, KEngineCore::Timer* timer, KEngineCore::StringHash buttonName, float frequency, std::function<void()> callback, std::function<void()> cancelCallback = nullptr);
		void Deinit();

		typedef std::list<ButtonHoldBinding*>::iterator Position;
		void SetPosition(Position position);
		Position GetPosition();

		KEngineCore::StringHash GetButtonName() const;

		void ButtonDown();
		void ButtonUp();

		void Cancel();
	private:

		void Fire();

		Input*					mInputSystem{ nullptr };
		KEngineCore::Timer*		mTimer{ nullptr };
		KEngineCore::StringHash	mButtonName;
		Position				mPosition;
		float					mFrequency;
		KEngineCore::Timeout	mTimeout;
		std::function<void()>	mCallback;
		std::function<void()>	mCancelCallback;
	};

	class AxisBinding
	{
	public:
		AxisBinding();
		~AxisBinding();
		void Init(Input* inputSystem, KEngineCore::Timer* timer, KEngineCore::StringHash controlName, float deadZone, float frequency, std::function<void(float)> callback, std::function<void()> cancelCallback = nullptr);
		void Deinit();

		typedef std::list<AxisBinding*>::iterator Position;
		void SetPosition(Position position);
		Position GetPosition();

		KEngineCore::StringHash GetAxisName() const;

		void UpdateAxis(float tilt);

		void Fire(float tilt);
		void Cancel();

	private:
		Input*						mInputSystem{ nullptr };
		KEngineCore::Timer*			mTimer{ nullptr };
		KEngineCore::StringHash		mAxisName;
		Position					mPosition;
		float						mDeadZone;
		float						mFrequency;
		std::function<void(float)>	mCallback;
		std::function<void()>		mCancelCallback;

		ButtonDownBinding			mNegativeDown;
		ButtonUpBinding				mNegativeUp;

		ButtonDownBinding			mPositiveDown;
		ButtonUpBinding				mPositiveUp;

		float						mLastTilt{ 0.0f };
		bool						mDead{ true };
		KEngineCore::Timeout		mTimeout;
	};


	class CombinedAxisBinding
	{
	public:
		CombinedAxisBinding();
		~CombinedAxisBinding(); 
		void Init(Input* inputSystem, KEngineCore::Timer* timer, KEngineCore::StringHash controlName, float deadZone, float frequncy, std::function<void(const KEngine2D::Point&)> callback, std::function<void()> cancelCallback = nullptr);
		void Deinit();

		typedef std::list<CombinedAxisBinding*>::iterator Position;
		void SetPosition(Position position);
		Position GetPosition();

		KEngineCore::StringHash GetControlName() const;

		void Fire(const KEngine2D::Point & tilt);
		void Cancel();
	private:


		Input*											mInputSystem{ nullptr };
		KEngineCore::Timer*								mTimer{ nullptr };
		KEngineCore::StringHash							mControlName;
		bool											mIsVerticalAxis;
		Position										mPosition;
		float											mDeadZone;
		float											mFrequency;
		std::function<void(const KEngine2D::Point&)>	mCallback;
		std::function<void()>							mCancelCallback;

		AxisBinding	mHorizontalAxisBinding;
		AxisBinding	mVerticalAxisBinding;

		KEngine2D::Point		mLastTilt;
		bool					mDead{ true };
		KEngineCore::Timeout	mTimeout;
	};

	class Input : private KEngineCore::LuaLibrary
	{
	public:
		Input();
		virtual ~Input();
		void Init(KEngineCore::LuaScheduler* scheduler, KEngineCore::Timer* timer);
		void Deinit();

		void AddCombinedAxis(KEngineCore::StringHash name);
		void AddChildAxis(KEngineCore::StringHash parentName, KEngineCore::StringHash name, AxisType axisType, ControllerType controllerType, int id);
		void AddAxis(KEngineCore::StringHash name, ControllerType controllerType, int id);
		void AddAxisButton(KEngineCore::StringHash axisName, KEngineCore::StringHash name, AxisType axisType, int direction, ControllerType controllerType, int id);
		void AddButton(KEngineCore::StringHash name, ControllerType controllerType, int id);

		void AddCombinedAxisBinding(CombinedAxisBinding* binding);
		void AddAxisBinding(AxisBinding* binding);
		void AddButtonDownBinding(ButtonDownBinding* binding);
		void AddButtonHoldBinding(ButtonHoldBinding* binding);
		void AddButtonUpBinding(ButtonUpBinding* binding);

		bool RemoveCombinedAxisBinding(CombinedAxisBinding* binding);
		bool RemoveAxisBinding(AxisBinding* binding);
		bool RemoveButtonDownBinding(ButtonDownBinding* binding);
		bool RemoveButtonUpBinding(ButtonUpBinding* binding);
		bool RemoveButtonHoldBinding(ButtonHoldBinding* binding);

		void HandleAxisChange(ControllerType type, int axisId, float axisPosition);
		void HandleButtonDown(ControllerType type, int buttonId);
		void HandleButtonUp(ControllerType type, int buttonId);		

		bool HasCombinedAxis(KEngineCore::StringHash name) const;
		bool HasChildAxis(KEngineCore::StringHash parentName, AxisType axisType) const;
		bool HasAxis(KEngineCore::StringHash name) const;
		bool HasAxisButton(KEngineCore::StringHash parentName, int direction) const;
		bool HasButton(KEngineCore::StringHash name) const;

		KEngineCore::StringHash GetAxisForCombinedAxis(KEngineCore::StringHash combinedAxisName, AxisType axisType) const;
		KEngineCore::StringHash GetButtonForAxis(KEngineCore::StringHash AxisName, int direction) const;

		virtual void RegisterLibrary(lua_State* luaState, char const* name = "input") override;
	private:

		bool HasAxisMapping(ControllerType type, int axisId) const;
		bool HasButtonMapping(ControllerType type, int axisId) const;

		KEngineCore::StringHash GetAxisMapping(ControllerType type, int axisId) const;
		KEngineCore::StringHash GetButtonMapping(ControllerType type, int buttonId) const;

		template<typename BindingType>
		struct BindingGroup
		{
			std::list<BindingType*>							mBindings;
			BindingType* mProcessingBinding; //THIS ISN'T GETTING USED YET
			std::vector<BindingType*>						mSelfClearedBindings;
			inline void Cleanup() {
				for (auto selfClearedBinding : mSelfClearedBindings)
				{
					mBindings.erase(selfClearedBinding->GetPosition());
				}
			}
		};

		struct ButtonBindingPack
		{
			BindingGroup<ButtonDownBinding> mButtonDownBindings;
			BindingGroup<ButtonUpBinding> mButtonUpBindings;
			BindingGroup<ButtonHoldBinding> mButtonHoldBindings;
		};

		BindingGroup<AxisBinding>& GetAxisBindings(KEngineCore::StringHash name);
		ButtonBindingPack& GetButtonBindings(KEngineCore::StringHash name);

		KEngineCore::LuaScheduler*	mScheduler{ nullptr };
		KEngineCore::Timer*			mTimer{ nullptr };

		std::set<KEngineCore::StringHash>	mCombinedAxes;
		std::set<KEngineCore::StringHash>	mAxes;
		std::set<KEngineCore::StringHash>	mButtons;

		typedef std::pair<KEngineCore::StringHash, AxisType> ChildAxisDescription;
		std::map<ChildAxisDescription, KEngineCore::StringHash> mChildAxes;

		typedef std::pair<KEngineCore::StringHash, int> AxisButtonDescription;
		std::map<AxisButtonDescription, KEngineCore::StringHash> mAxisButtons;
		
		typedef std::pair<ControllerType, int> PhysicalControlDescription;
		std::map<PhysicalControlDescription, KEngineCore::StringHash> mAxisMappings;
		std::map<PhysicalControlDescription, KEngineCore::StringHash> mButtonMappings;
		
		std::map<KEngineCore::StringHash, ButtonBindingPack> mButtonBindings;
		std::map<KEngineCore::StringHash, BindingGroup<AxisBinding>> mAxisBindings;
		BindingGroup<CombinedAxisBinding>	mCombinedAxisBindings;
	};
}