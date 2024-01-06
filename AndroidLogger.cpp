//
//  StdLogger.cpp
//  KEngineBasics
//
//  Created by Kelson Hootman on 12/29/23.
//

#include "AndroidLogger.h"
#include "Logger.h"
#include <android/log.h>

#define APPNAME "KEngine" //TODO:  pass in?


KEngineBasics::AndroidLogger::AndroidLogger()
{
    
}

KEngineBasics::AndroidLogger::~AndroidLogger()
{
    Deinit();
}

void KEngineBasics::AndroidLogger::Init(KEngineCore::Logger *coreLogger)
{
    coreLogger->AddErrorHandler([] (std::string_view logString) {
        __android_log_print(ANDROID_LOG_ERROR, APPNAME, "%s", std::string(logString).data());
    });
    
    coreLogger->AddLogHandler([] (std::string_view logString) {
        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "%s", std::string(logString).data());
    });
}

void KEngineBasics::AndroidLogger::Deinit()
{
    //TODO (maybe):  clean up after yourself.
}

