//
//  AndroidLogger.h
//  KEngineBasics
//
//  Created by Kelson Hootman on 12/29/23.
//

#pragma once

namespace KEngineCore
{
    class Logger;
}

namespace KEngineBasics
{
    class AndroidLogger {
    public:
        AndroidLogger();
        ~AndroidLogger();
        void Init(KEngineCore::Logger * coreLogger);
        void Deinit();
    };
    
}
