//
//  StdLogger.h
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
    class StdLogger {
    public:
        StdLogger();
        ~StdLogger();
        void Init(KEngineCore::Logger * coreLogger);
        void Deinit();
    };
    
}
