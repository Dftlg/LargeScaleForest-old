#include"LoadWindSourceConfig.h"

CLoadWindSourceConfig::CLoadWindSourceConfig(const std::string & vFilePath)
{
    __readWindSourceConfig(vFilePath);
}

void CLoadWindSourceConfig::__readWindSourceConfig(const std::string &vFilePath)
{
    std::ifstream positionFile(vFilePath);
    std::string lineString;
    while (getline(positionFile, lineString))
    {
        std::istringstream sin(lineString);
        std::string str;
        sin >> str;
        if (str == "WindCenter")
        {
            getline(positionFile, lineString);
            std::vector<std::string> WindCenter;
            boost::split(WindCenter, lineString, boost::is_any_of(","), boost::token_compress_off);
            m_windSourceCenter.x = atof(WindCenter[0].c_str());
            m_windSourceCenter.y = atof(WindCenter[1].c_str());
            m_windSourceCenter.z = atof(WindCenter[2].c_str());
        }
        if (str == "WindInfluenceArea")
        {
            getline(positionFile, lineString);
            m_windShpere4Influence = atof(lineString.c_str());
        }
        if (str == "WindCenterMoveVelocity")
        {
            getline(positionFile, lineString);
            int VelocityNumber= atoi(lineString.c_str());
            glm::vec3 tempVec3Velocity;
            for (int i = 0; i < VelocityNumber; i++)
            {
                std::vector<std::string> MoveVelocity;
                getline(positionFile, lineString);
                boost::split(MoveVelocity, lineString, boost::is_any_of(","), boost::token_compress_off);
                m_windCenterMoveVelocity.push_back(glm::vec3(atof(MoveVelocity[0].c_str()), atof(MoveVelocity[1].c_str()), atof(MoveVelocity[2].c_str())));
            }
        }
        if (str == "WindCenterMoveFrames")
        {
            std::vector<std::string> MoveFrames;
            getline(positionFile, lineString);
            boost::split(MoveFrames, lineString, boost::is_any_of(","), boost::token_compress_off);
            for (auto moveFrame : MoveFrames)
                m_windCenterMoveFrames.push_back((atoi(moveFrame.c_str())));
        }
        if (str == "WindCenterMoveScales")
        {
            std::vector<std::string> MoveScale;
            getline(positionFile, lineString);
            boost::split(MoveScale, lineString, boost::is_any_of(","), boost::token_compress_off);
            for (auto movescale : MoveScale)
                m_windCenterMoveFrames.push_back((atof(movescale.c_str())));
        }
        if (str == "WindPara")
        {
            getline(positionFile, lineString);
            int ParaNumber = atoi(lineString.c_str());
            for (int i = 0; i < ParaNumber; i++)
            {
                std::vector<std::string> ForceConfig;
                getline(positionFile, lineString);
                boost::split(ForceConfig, lineString, boost::is_any_of(","), boost::token_compress_off);
                SWaveFunctionPara oneWavePara(ForceConfig);
                m_specificWindFunctionPara.push_back(oneWavePara);
            }
        }
    }
}


