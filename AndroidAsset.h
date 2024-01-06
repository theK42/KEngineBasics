//
//  AndroidAsset.h
//  KEngineBasics
//
//  Created by Kelson Hootman on 12/30/23.
//

#pragma once


namespace KEngineBasics
{

    class AndroidAssetStreamBuf: public std::streambuf
    {
    public:
        AndroidAssetStreamBuf();
        virtual ~AndroidAssetStreamBuf();
        void Init(AAssetManager* manager, const std::string_view filename);
        void Deinit();
    protected:
        AAsset* mAsset;
        std::vector<char> mBuffer;
    
        std::streambuf::int_type underflow() override;
       

        std::streambuf::int_type overflow(std::streambuf::int_type value) override
        {
            return traits_type::eof();
        };

        int sync() override
        {
            std::streambuf::int_type result = overflow(traits_type::eof());

            return traits_type::eq_int_type(result, traits_type::eof()) ? -1 : 0;
        }
    };


    class AndroidAssetStream: public std::istream
    {
    public:
        AndroidAssetStream(AAssetManager* manager, const std::string& file)
        : std::istream(new AndroidAssetStream(manager, file))
        {
        }

        virtual ~AndroidAssetStream()
        {
            delete rdbuf();
        }
    };



    
}
