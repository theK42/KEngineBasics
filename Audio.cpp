#include "Audio.h"
#include "LuaScheduler.h"
#include "Logger.h"

KEngineBasics::AudioSystem::AudioSystem()
{
}

KEngineBasics::AudioSystem::~AudioSystem()
{
	Deinit();
}

void KEngineBasics::AudioSystem::Init(KEngineCore::LuaScheduler* luaScheduler, KEngineCore::Logger * logger)
{
    assert(mLuaScheduler == nullptr);
    mLogger = logger;
	int status = Mix_Init(MIX_INIT_MP3);
    if ((status & MIX_INIT_MP3) == 0)
    {
        mLogger->LogError("Failed to get MP3 support");
        mLogger->LogError("Mix_Init error: {}", Mix_GetError());
    }
    status = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048);
    assert(status == 0);
	mLuaScheduler = luaScheduler;
	RegisterLibrary(luaScheduler->GetMainState());
}

void KEngineBasics::AudioSystem::Deinit()
{
	for (auto pair : mLoadedMusic)
	{
		if (pair.second != nullptr)
		{
			Mix_FreeMusic(pair.second);
		}
	}
	mLoadedMusic.clear();
	for (auto pair : mLoadedSounds)
	{
		if (pair.second != nullptr)
		{
			Mix_FreeChunk(pair.second);
		}
	}
	mLoadedSounds.clear();
	Mix_CloseAudio();
}

void KEngineBasics::AudioSystem::RegisterLibrary(lua_State* luaState, char const* name)
{
	auto luaopen_audio = [](lua_State* luaState) {
		auto playMusic = [](lua_State* luaState) {
			AudioSystem* audioSystem = (AudioSystem*)lua_touserdata(luaState, lua_upvalueindex(1));
			KEngineCore::LuaScheduler* scheduler = audioSystem->mLuaScheduler;
			KEngineCore::StringHash musicName(luaL_checkstring(luaState, 1));
			audioSystem->PlayMusic(musicName);
			return 0;
		};


		auto playSound = [](lua_State* luaState) {
			AudioSystem* audioSystem = (AudioSystem*)lua_touserdata(luaState, lua_upvalueindex(1));
			KEngineCore::LuaScheduler* scheduler = audioSystem->mLuaScheduler;
			KEngineCore::StringHash soundName(luaL_checkstring(luaState, 1));
			audioSystem->PlaySound(soundName);
			return 0;
		};
		
		const luaL_Reg audioLibrary[] = {
			{"playMusic", playMusic},
			{"playSound", playSound},
			{nullptr, nullptr}
		};
		
		luaL_newlibtable(luaState, audioLibrary);
		lua_pushvalue(luaState, lua_upvalueindex(1));
		luaL_setfuncs(luaState, audioLibrary, 1);
		return 1;
	};

	PreloadLibrary(luaState, name, luaopen_audio);
}

void KEngineBasics::AudioSystem::LoadMusic(KEngineCore::StringHash musicId, const std::string& filename)
{
	Mix_Music* m = Mix_LoadMUS(filename.c_str());
    if (!m)
    {
        mLogger->LogError("Music file failed to load: {}", filename.c_str());
        mLogger->LogError("Mix_LoadMUS error: {}", Mix_GetError());
    } else {
        mLoadedMusic[musicId] = m;
    }
}

void KEngineBasics::AudioSystem::UnloadMusic(KEngineCore::StringHash musicId)
{
	auto it = mLoadedMusic.find(musicId);
	if (it != mLoadedMusic.end() && it->second != nullptr)
	{
		Mix_Music* m = it->second;
		Mix_FreeMusic(m);
		mLoadedMusic.erase(it);
	}	
}

void KEngineBasics::AudioSystem::LoadSound(KEngineCore::StringHash soundId, const std::string& filename)
{
	Mix_Chunk* s = Mix_LoadWAV(filename.c_str());
	assert(s);
	mLoadedSounds[soundId] = s;
}


void KEngineBasics::AudioSystem::UnloadSound(KEngineCore::StringHash soundId)
{
	auto it = mLoadedSounds.find(soundId);
	if (it != mLoadedSounds.end() && it->second != nullptr)
	{
		Mix_Chunk* s = it->second;
		Mix_FreeChunk(s);
		mLoadedSounds.erase(it);
	}
}

void KEngineBasics::AudioSystem::PlayMusic(KEngineCore::StringHash musicId, bool loop, std::function<void()> onComplete)
{
	auto it = mLoadedMusic.find(musicId);
	if (it != mLoadedMusic.end() && it->second != nullptr)
	{
		Mix_Music* m = it->second;
		Mix_PlayMusic(m, loop ? -1 : 0);
	}
}

KEngineBasics::Sound* KEngineBasics::AudioSystem::PlaySound(KEngineCore::StringHash soundId, bool isVoice, std::function<void()> onComplete)
{
	//TODO:  make this sound pauseable, respect voice boolean, support onComplete
	auto it = mLoadedSounds.find(soundId);
	if (it != mLoadedSounds.end() && it->second != nullptr)
	{
		Mix_Chunk* s = it->second;
		int channel = Mix_PlayChannel(-1, s, 0);
        if (channel == -1)
        {
            mLogger->LogError("Mix_PlayChannel error: {}\n", Mix_GetError());
        }
	}
	return nullptr;
}

KEngineBasics::Sound::Sound()
{
}

KEngineBasics::Sound::~Sound()
{
	Deinit();
}

void KEngineBasics::Sound::Init(int channelId)
{
	mChannelId = channelId;
}

void KEngineBasics::Sound::Deinit()
{
}
