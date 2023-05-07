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
	class InputForwarder;


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
		void Init(Input* inputSystem, KEngineCore::StringHash buttonName, std::function<void()> callback, std::function<void()> cancelCallback = nullptr, bool oneShot = false);
		void Deinit();

		typedef std::list<ButtonDownBinding*>::iterator Position;
		void SetPosition(Position position);
		Position GetPosition();

		KEngineCore::StringHash GetButtonName() const;

		void Fire();
		void Cancel();
		static const char MetaName[];
	private:
		Input*					mInputSystem{ nullptr };
		KEngineCore::StringHash mButtonName;
		Position				mPosition;
		std::function<void()>	mCallback;
		std::function<void()>	mCancelCallback;
		bool					mOneShot;
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
		static const char MetaName[];
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
		static const char MetaName[];
	private:

		void Fire();

		Input*					mInputSystem{ nullptr };
		KEngineCore::Timer*		mTimer{ nullptr };
		KEngineCore::StringHash	mButtonName;
		Position				mPosition;
		float					mFrequency{ 0.0f };
		KEngineCore::Timeout	mTimeout;
		std::function<void()>	mCallback{ nullptr };
		std::function<void()>	mCancelCallback{ nullptr };
		bool					mButtonIsDown{ false };
		bool					mButtonIsReady{ true };
		ButtonDownBinding		mButtonDown;
		ButtonUpBinding			mButtonUp;
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

		static const char MetaName[];
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
		static const char MetaName[];

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

	class Input
	{
	public:
		Input();
		~Input();
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

		void AddInputForwarder(InputForwarder* forwarder);
		void RemoveInputForwarder(InputForwarder* forwarder);

		void Pause();
		void Resume();
	private:

		bool HasAxisMapping(ControllerType type, int axisId) const;
		bool HasButtonMapping(ControllerType type, int axisId) const;

		KEngineCore::StringHash GetAxisMapping(ControllerType type, int axisId) const;
		KEngineCore::StringHash GetButtonMapping(ControllerType type, int buttonId) const;

		void HandleButonDownInternal(KEngineBasics::ControllerType type, int buttonId);
		void HandleButtonUpInternal(KEngineBasics::ControllerType type, int buttonId);

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
					selfClearedBinding->SetPosition(mBindings.end());
				}
				mSelfClearedBindings.clear();
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

		std::list<InputForwarder*>			mForwarders;

		bool								mPaused { false };
		
		struct ControlID
		{
			ControllerType	type;
			int				id;

			bool operator<(const ControlID& r) const {
				return (type < r.type || (type == r.type && id < r.id));
			}
		};
		
		std::map<ControlID, int>	mQueuedAxisUpdates;
		std::set<ControlID>			mQueuedButtonDowns;
		std::set<ControlID>			mQueuedButtonUps;

		friend class InputLibrary;
	};

	class InputForwarder
	{
	public:
		InputForwarder();
		~InputForwarder();
		void Init(Input* input, std::function<void(ControllerType, int, int)> axisCallback, std::function<void(ControllerType, int)>buttonDownCallback, std::function<void(ControllerType, int)>buttonUpCallback);
		void Deinit(bool batched = false);

		void HandleAxisChange(ControllerType type, int axisId, float axisPosition);
		void HandleButtonDown(ControllerType type, int buttonId);
		void HandleButtonUp(ControllerType type, int buttonId);
	private:
		Input* mInput{ nullptr };
		std::list<InputForwarder*>::iterator	mPosition;
		std::function<void(double)>				mCallback{ nullptr };
		friend class Input;

		std::function<void(ControllerType, int, int)>	mAxisCallback;
		std::function<void(ControllerType, int)>		mButtonDownCallback;
		std::function<void(ControllerType, int)>		mButtonUpCallback;
	};

	class InputLibrary : public KEngineCore::LuaLibraryTwo<Input>
	{
	public:
		InputLibrary();
		~InputLibrary();
		void Init(lua_State* luaState);
		void Deinit();
	};

}