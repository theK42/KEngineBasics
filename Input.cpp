#include "Input.h"
#include "LuaScheduler.h"
#include <StringHash.h>
#include <algorithm>

using namespace KEngineBasics;

Input::Input()
{

}


Input::~Input()
{
	Deinit();
}

void Input::Init(KEngineCore::LuaScheduler* scheduler, KEngineCore::Timer* timer)
{
	assert(mScheduler == nullptr);
	mScheduler = scheduler;
	mTimer = timer;
	RegisterLibrary(scheduler->GetMainState());
}


void Input::RegisterLibrary(lua_State * luaState, const char * name)
{
	auto luaopen_input = [](lua_State* luaState) {
		auto setOnCombinedAxisTilt = [](lua_State* luaState) {
			Input* inputSystem = (Input*)lua_touserdata(luaState, lua_upvalueindex(1));
			KEngineCore::LuaScheduler* scheduler = inputSystem->mScheduler;
			KEngineCore::StringHash axisName(luaL_checkstring(luaState, 1));

			luaL_checktype(luaState, 2, LUA_TFUNCTION);

			CombinedAxisBinding* binding = new (lua_newuserdata(luaState, sizeof(CombinedAxisBinding))) CombinedAxisBinding;
			luaL_getmetatable(luaState, "CombinedAxisBinding");
			lua_setmetatable(luaState, -2);
			float deadZone = 0.1;
			float frequency = 30;

			KEngineCore::ScheduledLuaCallback<KEngine2D::Point> callback = scheduler->CreateCallback<KEngine2D::Point>(luaState, 2);
			binding->Init(inputSystem, inputSystem->mTimer, axisName, deadZone, frequency, callback.mCallback, callback.mCancelCallback);
			return 1;
		};

		const struct luaL_Reg inputLibrary[] = {
			{"setOnCombinedAxisTilt", setOnCombinedAxisTilt},
			{nullptr, nullptr}
		};

		auto deleteCombinedAxisBinding = [](lua_State* luaState) {
			CombinedAxisBinding* binding = (CombinedAxisBinding*)luaL_checkudata(luaState, 1, "KEngineBasics.CombinedAxisBinding");
			binding->~CombinedAxisBinding();
			return 0;
		};

		lua_checkstack(luaState, 3);
		luaL_newmetatable(luaState, "KEngineBasics.CombinedAxisBinding");
		lua_pushstring(luaState, "__gc");
		lua_pushcfunction(luaState, deleteCombinedAxisBinding);
		lua_settable(luaState, -3);
		lua_pop(luaState, 1);

		luaL_newlibtable(luaState, inputLibrary);
		lua_pushvalue(luaState, lua_upvalueindex(1));
		luaL_setfuncs(luaState, inputLibrary, 1);
		return 1;
	};


	PreloadLibrary(luaState, name, luaopen_input);
}

void Input::Deinit()
{
	for (auto binding : mCombinedAxisBindings.mBindings)
	{
		binding->Deinit();
	}
	mCombinedAxisBindings.mBindings.clear();

	for (auto& bindingGroupPair : mAxisBindings)
	{
		auto& bindingGroup = bindingGroupPair.second;
		for (auto binding : bindingGroup.mBindings)
		{
			binding->Deinit();
		}
	}
	mAxisBindings.clear();

	for (auto& bindingPackPair : mButtonBindings)
	{
		auto& bindingPack = bindingPackPair.second;
		for (auto binding : bindingPack.mButtonHoldBindings.mBindings)
		{
			binding->Deinit();
		}
		for (auto binding : bindingPack.mButtonUpBindings.mBindings)
		{
			binding->Deinit();
		}
		for (auto binding : bindingPack.mButtonDownBindings.mBindings)
		{
			binding->Deinit();
		}
	}
	mButtonBindings.clear();

	mButtonMappings.clear();
	mAxisMappings.clear();
	mChildAxes.clear();

	mButtons.clear();
	mAxes.clear();
	mCombinedAxes.clear();

	mTimer = nullptr;
}

void Input::AddCombinedAxis(KEngineCore::StringHash name)
{
	mCombinedAxes.insert(name);
}

void Input::AddChildAxis(KEngineCore::StringHash parentName, KEngineCore::StringHash name, AxisType axisType, ControllerType controllerType, int id)
{
	AddAxis(name, controllerType, id);
	mChildAxes[{ parentName, axisType }] = name;
}

void Input::AddAxis(KEngineCore::StringHash name, ControllerType controllerType, int id)
{
	mAxes.insert(name);
	mAxisMappings[{ controllerType, id }] = name;
	mAxisBindings[name] =  {};
}

void Input::AddAxisButton(KEngineCore::StringHash axisName, KEngineCore::StringHash name, AxisType axisType, int direction, ControllerType controllerType, int id)
{
	AddButton(name, controllerType, id);
	mAxisButtons[{axisName, direction}] = name;
}

void Input::AddButton(KEngineCore::StringHash name, ControllerType controllerType, int id)
{
	mButtons.insert(name);
	mButtonMappings[{ controllerType, id }] = name;
	mButtonBindings[name] = {};
}


void Input::AddCombinedAxisBinding(CombinedAxisBinding* binding)
{
	mCombinedAxisBindings.mBindings.push_back(binding);
	binding->SetPosition(std::next(mCombinedAxisBindings.mBindings.rbegin()).base());
}

void Input::AddAxisBinding(AxisBinding* binding)
{
	assert(HasAxis(binding->GetAxisName()));
	auto& bindingGroup = GetAxisBindings(binding->GetAxisName());
	bindingGroup.mBindings.push_back(binding);
	binding->SetPosition(std::next(bindingGroup.mBindings.rbegin()).base());
}

void Input::AddButtonDownBinding(ButtonDownBinding* binding)
{
	assert(HasButton(binding->GetButtonName()));
	auto& bindingGroup = GetButtonBindings(binding->GetButtonName()).mButtonDownBindings;
	bindingGroup.mBindings.push_back(binding);
	binding->SetPosition(std::next(bindingGroup.mBindings.rbegin()).base());
}

void Input::AddButtonUpBinding(ButtonUpBinding* binding)
{
	assert(HasButton(binding->GetButtonName()));
	auto& bindingGroup = GetButtonBindings(binding->GetButtonName()).mButtonUpBindings;
	bindingGroup.mBindings.push_back(binding);
	binding->SetPosition(std::next(bindingGroup.mBindings.rbegin()).base());
}

void Input::AddButtonHoldBinding(ButtonHoldBinding* binding)
{
	assert(HasButton(binding->GetButtonName()));
	auto& bindingGroup = GetButtonBindings(binding->GetButtonName()).mButtonHoldBindings;
	bindingGroup.mBindings.push_back(binding);
	binding->SetPosition(std::next(bindingGroup.mBindings.rbegin()).base());
}

bool Input::RemoveCombinedAxisBinding(CombinedAxisBinding* binding)
{
	if (mCombinedAxisBindings.mBindings.end() != binding->GetPosition())
	{
		mCombinedAxisBindings.mBindings.erase(binding->GetPosition());
		return true;
	}
	return false;
}

bool Input::RemoveAxisBinding(AxisBinding* binding)
{
	assert(HasAxis(binding->GetAxisName()));
	auto& bindingGroup = GetAxisBindings(binding->GetAxisName());
	if (bindingGroup.mBindings.end() != binding->GetPosition())
	{
		bindingGroup.mBindings.erase(binding->GetPosition());
		return true;
	}
	return false;
}

bool Input::RemoveButtonDownBinding(ButtonDownBinding* binding)
{
	assert(HasAxis(binding->GetButtonName()));
	auto& bindingGroup = GetButtonBindings(binding->GetButtonName()).mButtonDownBindings;
	if (bindingGroup.mBindings.end() != binding->GetPosition())
	{
		bindingGroup.mBindings.erase(binding->GetPosition());
		return true;
	}
	return false;
}

bool Input::RemoveButtonHoldBinding(ButtonHoldBinding* binding)
{
	assert(HasAxis(binding->GetButtonName()));
	auto& bindingGroup = GetButtonBindings(binding->GetButtonName()).mButtonHoldBindings;
	if (bindingGroup.mBindings.end() != binding->GetPosition())
	{
		bindingGroup.mBindings.erase(binding->GetPosition());
		return true;
	}
	return false;
}

bool Input::RemoveButtonUpBinding(ButtonUpBinding* binding)
{
	assert(HasAxis(binding->GetButtonName()));
	auto& bindingGroup = GetButtonBindings(binding->GetButtonName()).mButtonUpBindings;
	if (bindingGroup.mBindings.end() != binding->GetPosition())
	{
		bindingGroup.mBindings.erase(binding->GetPosition());
		return true;
	}
	return false;
}


ButtonDownBinding::ButtonDownBinding()
{
}

ButtonDownBinding::~ButtonDownBinding()
{
	Deinit();
}

void ButtonDownBinding::Init(Input* inputSystem, KEngineCore::StringHash buttonName, std::function<void()> callback, std::function<void()> cancelCallback)
{
	assert(mInputSystem == nullptr);
	mInputSystem = inputSystem;
	mButtonName = buttonName;
	mCallback = callback;
	mCancelCallback = cancelCallback;
	inputSystem->AddButtonDownBinding(this);
}

void ButtonDownBinding::Deinit()
{
	Cancel();
	mInputSystem = nullptr;
}

void ButtonDownBinding::SetPosition(ButtonDownBinding::Position position)
{
	mPosition = position;
}

ButtonDownBinding::Position ButtonDownBinding::GetPosition()
{
	return mPosition;
}

KEngineCore::StringHash KEngineBasics::ButtonDownBinding::GetButtonName() const
{
	return mButtonName;
}

void ButtonDownBinding::Fire()
{
	assert(mCallback);
	mCallback();
}

void ButtonDownBinding::Cancel()
{
	if (mInputSystem != nullptr)
	{
		if (mInputSystem->RemoveButtonDownBinding(this) && mCancelCallback) {
			mCancelCallback();
		}
	}
}

ButtonUpBinding::ButtonUpBinding()
{
}

ButtonUpBinding::~ButtonUpBinding()
{
	Deinit();
}

void ButtonUpBinding::Init(Input* inputSystem, KEngineCore::StringHash buttonName, std::function<void()> callback, std::function<void()> cancelCallback)
{
	assert(mInputSystem == nullptr);
	mInputSystem = inputSystem;
	mButtonName = buttonName;
	mCallback = callback;
	mCancelCallback = cancelCallback;
	inputSystem->AddButtonUpBinding(this);
}

void ButtonUpBinding::Deinit()
{
	Cancel();
}

void ButtonUpBinding::SetPosition(ButtonUpBinding::Position position)
{
	mPosition = position;
}

ButtonUpBinding::Position ButtonUpBinding::GetPosition()
{
	return mPosition;
}

KEngineCore::StringHash KEngineBasics::ButtonUpBinding::GetButtonName() const
{
	return mButtonName;
}

void ButtonUpBinding::Fire()
{
	assert(mCallback);
	mCallback();
}

void ButtonUpBinding::Cancel()
{
	if (mInputSystem != nullptr)
	{
		if (mInputSystem->RemoveButtonUpBinding(this) && mCancelCallback) {
			mCancelCallback();
		}
	}
}

ButtonHoldBinding::ButtonHoldBinding()
{
}

ButtonHoldBinding::~ButtonHoldBinding()
{
	Deinit();
}

void ButtonHoldBinding::Init(Input* inputSystem, KEngineCore::Timer * timer, KEngineCore::StringHash buttonName, float frequency, std::function<void()> callback, std::function<void()> cancelCallback)
{
	assert(mInputSystem == nullptr);
	mInputSystem = inputSystem;
	mButtonName = buttonName;
	mCallback = callback;
	mCancelCallback = cancelCallback;
	inputSystem->AddButtonHoldBinding(this);
	mTimer = timer;
	mFrequency = frequency;
}

void ButtonHoldBinding::Deinit()
{
	Cancel();
}

void ButtonHoldBinding::SetPosition(ButtonHoldBinding::Position position)
{
	mPosition = position;
}

ButtonHoldBinding::Position ButtonHoldBinding::GetPosition()
{
	return mPosition;
}

KEngineCore::StringHash KEngineBasics::ButtonHoldBinding::GetButtonName() const
{
	return mButtonName;
}

void KEngineBasics::ButtonHoldBinding::ButtonDown()
{
	mTimeout.Init(mTimer, 1.0 / mFrequency, true, [this]() {
		Fire();
	});
}

void KEngineBasics::ButtonHoldBinding::ButtonUp()
{
	mTimeout.Cancel();
}

void ButtonHoldBinding::Fire()
{
	assert(mCallback);
	mCallback();
}

void ButtonHoldBinding::Cancel()
{
	if (mInputSystem != nullptr)
	{
		mTimeout.Cancel();
		if (mInputSystem->RemoveButtonHoldBinding(this) && mCancelCallback) {
			mCancelCallback();
		}
	}
}

KEngineBasics::AxisBinding::AxisBinding()
{
}

AxisBinding::~AxisBinding()
{
	Deinit();
}


void AxisBinding::Init(Input* inputSystem, KEngineCore::Timer* timer, KEngineCore::StringHash controlName, float deadZone, float frequency, std::function<void(float)> callback, std::function<void()> cancelCallback)
{
	assert(mInputSystem == nullptr);
	mInputSystem = inputSystem;
	mAxisName = controlName;
	mCallback = callback;
	mCancelCallback = cancelCallback;
	inputSystem->AddAxisBinding(this);
	mTimer = timer;
	mFrequency = frequency;
	mDeadZone = deadZone;
	mDead = true;
	mLastTilt = 0.0f;


	if (inputSystem->HasAxisButton(controlName, -1)) {
		KEngineCore::StringHash negativeButton = inputSystem->GetButtonForAxis(controlName, -1);
		mNegativeDown.Init(inputSystem, negativeButton, [this]()
			{
				UpdateAxis(std::clamp(mLastTilt - 1.0f, -1.0f, 1.0f));
			}
		);

		mNegativeUp.Init(inputSystem, negativeButton, [this]()
			{
				UpdateAxis(std::clamp(mLastTilt + 1.0f, -1.0f, 1.0f));
			}
		);
	}
	
	if (inputSystem->HasAxisButton(controlName, 1)) {
		KEngineCore::StringHash positiveButton = inputSystem->GetButtonForAxis(controlName, 1);
		mPositiveDown.Init(inputSystem, positiveButton, [this]()
			{
				UpdateAxis(std::clamp(mLastTilt + 1.0f, -1.0f, 1.0f));
			}
		);

		mPositiveUp.Init(inputSystem, positiveButton, [this]()
			{
				UpdateAxis(mLastTilt = std::clamp(mLastTilt - 1.0f, -1.0f, 1.0f));
			}
		);
	}

}

void AxisBinding::Deinit()
{
	Cancel();
}

void AxisBinding::SetPosition(AxisBinding::Position position)
{
	mPosition = position;
}

AxisBinding::Position AxisBinding::GetPosition()
{
	return mPosition;
}

KEngineCore::StringHash KEngineBasics::AxisBinding::GetAxisName() const
{
	return mAxisName;
}

void AxisBinding::UpdateAxis(float tilt)
{
	mLastTilt = tilt;
	if (mDead && (mLastTilt >= mDeadZone || mLastTilt <= -mDeadZone))
	{
		mLastTilt = tilt;
		mDead = false;
		Fire(mLastTilt);
		mTimeout.Init(mTimer, 1.0 / mFrequency, true, [this]() {
			if (mLastTilt < mDeadZone && mLastTilt > -mDeadZone)
			{
				mDead = true;
				Fire(0.0f);
				mTimeout.Cancel();
			}
			else {
				Fire(mLastTilt);
			}
		});
	}
}

void AxisBinding::Fire(float tilt)
{
	assert(mCallback);
	mCallback(tilt);
}

void AxisBinding::Cancel()
{
	if (mInputSystem != nullptr)
	{
		mTimeout.Cancel();
		if (mInputSystem->RemoveAxisBinding(this) && mCancelCallback) {
			mCancelCallback();
		}
	}
}

KEngineBasics::CombinedAxisBinding::CombinedAxisBinding()
{
}

CombinedAxisBinding::~CombinedAxisBinding()
{
	Deinit();
}


void CombinedAxisBinding::Init(Input* inputSystem, KEngineCore::Timer* timer, KEngineCore::StringHash controlName, float deadZone, float frequency, std::function<void(const KEngine2D::Point&)> callback, std::function<void()> cancelCallback)
{
	assert(mInputSystem == nullptr);
	mInputSystem = inputSystem;
	mControlName = controlName;
	mCallback = callback;
	mCancelCallback = cancelCallback;
	inputSystem->AddCombinedAxisBinding(this);
	mTimer = timer;
	mFrequency = frequency;
	mDeadZone = deadZone;
	mDead = true;
	mLastTilt = KEngine2D::Point::Origin();

	if (inputSystem->HasChildAxis(controlName, AxisType::Horizontal)) {
		KEngineCore::StringHash horizontalName = inputSystem->GetAxisForCombinedAxis(controlName, AxisType::Horizontal);
		mHorizontalAxisBinding.Init(inputSystem, timer, horizontalName, deadZone, frequency, [this](float tilt)
			{
				mLastTilt.x = tilt;
				if (mDead && (mLastTilt.x >= mDeadZone || mLastTilt.x <= -mDeadZone || mLastTilt.y >= mDeadZone || mLastTilt.y <= -mDeadZone))
				{
					mDead = false;
					Fire(mLastTilt);
					mTimeout.Init(mTimer, 1.0 / mFrequency, true, [this]() {
						if (mLastTilt.x < mDeadZone && mLastTilt.x > -mDeadZone && mLastTilt.y < mDeadZone && mLastTilt.y > -mDeadZone)
						{
							mDead = true;
							Fire({ 0.0, 0.0 });
							mTimeout.Cancel();
						}
						else {
							Fire(mLastTilt);
						}
					});
				}
			}
		);
	}

	if (inputSystem->HasChildAxis(controlName, AxisType::Vertical)) {
		KEngineCore::StringHash verticalName = inputSystem->GetAxisForCombinedAxis(controlName, AxisType::Vertical);
		mVerticalAxisBinding.Init(inputSystem, timer, verticalName, deadZone, frequency, [this](float tilt)
			{
				mLastTilt.y = tilt;
				if (mDead && (mLastTilt.x >= mDeadZone || mLastTilt.x <= -mDeadZone || mLastTilt.y >= mDeadZone || mLastTilt.y <= -mDeadZone))
				{
					mDead = false;
					Fire(mLastTilt);
					mTimeout.Init(mTimer, 1.0 / mFrequency, true, [this]() {
						if (mLastTilt.x < mDeadZone && mLastTilt.x > -mDeadZone && mLastTilt.y < mDeadZone && mLastTilt.y > -mDeadZone)
						{
							mDead = true;
							mTimeout.Cancel();
						}
						else {
							Fire(mLastTilt);
						}
					});
				}
			}
		);
	}
}

void CombinedAxisBinding::Deinit()
{
	Cancel();
}

void CombinedAxisBinding::SetPosition(CombinedAxisBinding::Position position)
{
	mPosition = position;
}

CombinedAxisBinding::Position CombinedAxisBinding::GetPosition()
{
	return mPosition;
}

KEngineCore::StringHash KEngineBasics::CombinedAxisBinding::GetControlName() const
{
	return mControlName;
}

void CombinedAxisBinding::Fire(const KEngine2D::Point& tilt)
{
	assert(mCallback);
	mCallback(tilt);
}

void CombinedAxisBinding::Cancel()
{
	if (mInputSystem != nullptr)
	{
		mTimeout.Cancel();
		if (mInputSystem->RemoveCombinedAxisBinding(this) && mCancelCallback) {
			mCancelCallback();
		}
	}
}

void KEngineBasics::Input::HandleAxisChange(ControllerType type, int axisId, float axisPosition)
{
	if (HasAxisMapping(type, axisId))
	{
		KEngineCore::StringHash axisName = GetAxisMapping(type, axisId);
		auto& bindingGroup = GetAxisBindings(axisName);
		for (AxisBinding* binding : bindingGroup.mBindings)
		{
			bindingGroup.mProcessingBinding = binding;
			binding->UpdateAxis(axisPosition);
		}
		bindingGroup.Cleanup();
	}
}

void KEngineBasics::Input::HandleButtonDown(ControllerType type, int buttonId)
{
	if (HasButtonMapping(type, buttonId))
	{
		KEngineCore::StringHash axisName = GetButtonMapping(type, buttonId);
		auto& bindingGroup = GetButtonBindings(axisName).mButtonDownBindings;
		for (ButtonDownBinding* binding : bindingGroup.mBindings)
		{
			bindingGroup.mProcessingBinding = binding;
			binding->Fire();
		}
		bindingGroup.Cleanup();
	}
}


void KEngineBasics::Input::HandleButtonUp(ControllerType type, int buttonId)
{
	if (HasButtonMapping(type, buttonId))
	{
		KEngineCore::StringHash axisName = GetButtonMapping(type, buttonId);
		auto& bindingGroup = GetButtonBindings(axisName).mButtonUpBindings;
		for (ButtonUpBinding* binding : bindingGroup.mBindings)
		{
			bindingGroup.mProcessingBinding = binding;
			binding->Fire();
		}
		bindingGroup.Cleanup();
	}
}


bool KEngineBasics::Input::HasCombinedAxis(KEngineCore::StringHash name) const
{
	return mCombinedAxes.find(name) != mCombinedAxes.end();
}

bool KEngineBasics::Input::HasChildAxis(KEngineCore::StringHash parentName, AxisType axisType) const
{
	return mChildAxes.find({ parentName, axisType }) != mChildAxes.end();
}

bool KEngineBasics::Input::HasAxis(KEngineCore::StringHash name) const
{
	return mAxes.find(name) != mAxes.end();
}

bool KEngineBasics::Input::HasAxisButton(KEngineCore::StringHash parentName, int direction) const
{
	return mAxisButtons.find({ parentName, direction }) != mAxisButtons.end();
}

bool KEngineBasics::Input::HasButton(KEngineCore::StringHash name) const
{
	return mButtons.find(name) != mButtons.end();
}

KEngineCore::StringHash KEngineBasics::Input::GetAxisForCombinedAxis(KEngineCore::StringHash combinedAxisName, AxisType axisType) const
{
	return mChildAxes.find({ combinedAxisName, axisType })->second;
}

KEngineCore::StringHash KEngineBasics::Input::GetButtonForAxis(KEngineCore::StringHash axisName, int direction) const
{
	return mAxisButtons.find({ axisName, direction })->second;
}


bool KEngineBasics::Input::HasAxisMapping(ControllerType type, int axisId) const
{
	return mAxisMappings.find({ type, axisId }) != mAxisMappings.end();
}

bool KEngineBasics::Input::HasButtonMapping(ControllerType type, int axisId) const
{
	return mButtonMappings.find({ type, axisId }) != mButtonMappings.end();
}

KEngineCore::StringHash KEngineBasics::Input::GetAxisMapping(ControllerType type, int axisId) const 
{
	return mAxisMappings.find({ type, axisId })->second;
}

KEngineCore::StringHash KEngineBasics::Input::GetButtonMapping(ControllerType type, int buttonId) const
{
	return mButtonMappings.find({ type, buttonId })->second;
}

KEngineBasics::Input::BindingGroup<AxisBinding>& KEngineBasics::Input::GetAxisBindings(KEngineCore::StringHash name)
{
	return mAxisBindings.find(name)->second;
}

KEngineBasics::Input::ButtonBindingPack& KEngineBasics::Input::GetButtonBindings(KEngineCore::StringHash name)
{
	return mButtonBindings.find(name)->second;
}