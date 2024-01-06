//
//  StdLogger.cpp
//  KEngineBasics
//
//  Created by Kelson Hootman on 12/29/23.
//

#include "StdLogger.h"
#include "Logger.h"
#include <iostream>

KEngineBasics::StdLogger::StdLogger()
{
    
}

KEngineBasics::StdLogger::~StdLogger()
{
    Deinit();
}

void KEngineBasics::StdLogger::Init(KEngineCore::Logger *coreLogger)
{
    coreLogger->AddErrorHandler([] (std::string_view logString) {
        std::cerr << logString << std::endl;
    });
    
    coreLogger->AddLogHandler([] (std::string_view logString) {
        std::cout << logString << std::endl;
    });
}

void KEngineBasics::StdLogger::Deinit()
{
    //TODO (maybe):  clean up after yourself.
}

