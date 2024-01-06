//
//  StdLogger.cpp
//  KEngineBasics
//
//  Created by Kelson Hootman on 12/29/23.
//

#include "AndroidAsset.h"



KEngineBasics::AndroidAssetStreamBuf::AndroidAssetStreamBuf()
{
    
}

KEngineBasics::AndroidAssetStreamBuf::~AndroidAssetStreamBuf()
{
    Deinit();
}

void KEngineBasics::AndroidAssetStreamBuf::Init(AAssetManager* manager, const std::string_view filename)
{
    mAsset = AAssetManager_open(manager, filename.c_str(), AASSET_MODE_STREAMING);
    mBuffer.resize(1024);

    setg(0, 0, 0);
    setp(&buffer.front(), &buffer.front() + buffer.size());
}

void KEngineBasics::AndroidAssetStreamBuf::Deinit()
{
    sync();
    AAsset_close(mAsset);
}

std::streambuf::int_type KEngineBasics::AndroidAssetStreamBuf::underflow() override
{
    auto bufferPtr = &buffer.front();
    auto counter = AAsset_read(asset, bufferPtr, buffer.size());

    if(counter == 0)
        return traits_type::eof();
    if(counter < 0) //error, what to do now?
        return traits_type::eof();

    setg(bufferPtr, bufferPtr, bufferPtr + counter);

    return traits_type::to_int_type(*gptr());
}

std::streambuf::int_type KEngineBasics::AndroidAssetStreamBuf::overflow(std::streambuf::int_type value) override
{
    return traits_type::eof();
}

int KEngineBasics::AndroidAssetStreamBuf::sync() override
{
    std::streambuf::int_type result = overflow(traits_type::eof());
    return traits_type::eq_int_type(result, traits_type::eof()) ? -1 : 0;
}

