#pragma once
#include <iostream>
#include<fstream>
#include<sstream>
#include<string>
#include <vector>
#include "../Common/WindFiled.h"

class CLoadWindSourceConfig
{
public:
    CLoadWindSourceConfig() = default;
    CLoadWindSourceConfig(const std::string & vFilePath);
    ~CLoadWindSourceConfig() = default;

    glm::vec3 getWindSourceCenter() { return m_windSourceCenter; };
    double getWindInfluenceArea() { return m_windShpere4Influence; };
    std::vector<SWaveFunctionPara> getWindSourceFunctionPara() { return m_specificWindFunctionPara; };
    std::vector<glm::vec3> getWindCenterMoveVelocity() { return m_windCenterMoveVelocity; };
    std::vector<int> getWindCenterMoveFrames() { return m_windCenterMoveFrames; };
    std::vector<double> getWindCenterMoveScale() { return m_moveScale; };
    //std::vector<std::vector<SWaveFunctionPara>> getMultipleTreeWindPara() { return m_ProductMultipleTreePara; };
private:

    void __readWindSourceConfig(const std::string &vFilePath);

    glm::vec3 m_windSourceCenter;
    double m_windShpere4Influence;
    std::vector<SWaveFunctionPara> m_specificWindFunctionPara;
    std::vector<glm::vec3> m_windCenterMoveVelocity;
    std::vector<int> m_windCenterMoveFrames;
    std::vector<double> m_moveScale;
};

