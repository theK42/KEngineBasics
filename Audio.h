#pragma once
#include "LuaLibrary.h"
#include "Pool.h"
#include "StringHash.h"
#ifdef __EMSCRIPTEN__
    #include "SDL2/SDL_mixer.h"
#else
    #include "SDL_mixer.h"
#endif
#include <functional>
#include <map>

namespace KEngineCore
{
	class LuaScheduler;
    class Logger;
}

namespace KEngineBasics {
	class Sound
	{
	public:
		Sound();
		~Sound();
		void Init(int channelId);
		void Deinit();

		bool IsPlaying();
		void PauseSound();
	private:
		int mChannelId;
	};

	class AudioSystem : public KEngineCore::LuaLibrary
	{
	public:
		AudioSystem();
		~AudioSystem();

		void Init(KEngineCore::LuaScheduler* luaScheduler, KEngineCore::Logger* logger);
		void Deinit();

		void RegisterLibrary(lua_State* luaState, char const* name = "audio");

		void LoadMusic(KEngineCore::StringHash musicId, const std::string& filename);
		void UnloadMusic(KEngineCore::StringHash musicId);
		void LoadSound(KEngineCore::StringHash soundId, const std::string& filename);
		void UnloadSound(KEngineCore::StringHash soundId);


		void PlayMusic(KEngineCore::StringHash musicId, bool loop = true, std::function<void()> onComplete = nullptr);
		void InterruptMusic(KEngineCore::StringHash musicId);
		void PauseMusic();
		void ResumeMusic();

		Sound * PlaySound(KEngineCore::StringHash soundId, bool isVoice = false, std::function<void()> onComplete = nullptr);
		void RecycleSound(Sound* sound);
	private:
		KEngineCore::LuaScheduler*	mLuaScheduler;
        KEngineCore::Logger*        mLogger;
		KEngineCore::Pool<Sound>	mSoundPool;

		std::map<KEngineCore::StringHash, Mix_Music*> mLoadedMusic;
		std::map<KEngineCore::StringHash, Mix_Chunk*> mLoadedSounds;

	};

}
