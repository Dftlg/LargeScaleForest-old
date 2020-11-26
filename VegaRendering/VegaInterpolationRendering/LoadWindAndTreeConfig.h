#pragma once
#include <iostream>
#include<fstream>
#include<sstream>
#include<string>
#include <vector>
#include "../Common/WindFiled.h"

class CLoadWindAndTreeConfig
{
public:
    CLoadWindAndTreeConfig()=default;
    CLoadWindAndTreeConfig(int vTreeNumber,const std::string & vFilePath);
    ~CLoadWindAndTreeConfig()=default;
    void readTreeParaConfig(const std::string &vFilePath);
    void productMultipleTreeWindPara();
    std::vector<std::vector<SWaveFunctionPara>> getMultipleTreeWindPara() { return m_ProductMultipleTreePara; };
    //std::vector<std::vector<SWaveFunctionPara>> getMultipleTreeWindPara() { return m_MultipleTreePara; };
    std::vector<int> getMultipleTreeDuplicateNumber() { return m_EachTypeTreeNumber; };
    std::vector<float> getMultipleRotationAngle() { return m_ProductRotationAngle; };

private:
    std::vector<std::vector<SWaveFunctionPara>> m_MultipleTreePara;
    std::vector<float> m_TreeRotationAngle;
    std::vector<int> m_EachTypeTreeNumber;

    std::vector<std::vector<SWaveFunctionPara>> m_ProductMultipleTreePara;

    //reproduce TreeRotationAngle
    std::vector<float> m_ProductRotationAngle;
    int m_TreeNumber;
};

