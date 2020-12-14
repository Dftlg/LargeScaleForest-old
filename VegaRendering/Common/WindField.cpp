#include"WindFiled.h"

CWindField::CWindField(const int vSize, const std::vector<SWaveFunctionPara> vSwavePara, const int vWavelength, Common::SForceDirection vWindDirection, Common::SForceDirection vTreeRotationDirection)
{
	std::vector<int>tempForces(vSize,0);
	for (auto tempwavepara : vSwavePara)
	{
        std::vector<int> temp = GenerateSamplingForce(vSize, tempwavepara.Amplitude, tempwavepara.Frequency, tempwavepara.Phase, tempwavepara.Ypluse, vWavelength);
        if (tempwavepara.Pulse == 1)
        {
            for (int i = 0; i < vSize; i++)
            {
                if (temp[i] < tempwavepara.Ypluse)
                {
                    temp[i] = tempwavepara.Ypluse;
                    if (tempwavepara.DropNumber != 0)
                    {
                        temp[i] -= tempwavepara.DropNumber;
                    }
                }
            }       
        }	
		for (int i = 0; i < vSize; i++)
		{
			tempForces[i] += temp[i];
		}
	}
    Common::SForceDirection tempDirection = caculateRelativeDirection(vWindDirection, vTreeRotationDirection);
	std::vector<Common::SForceDirection> tempDirections(vSize, tempDirection);
	m_Forces.push_back(tempForces);
    m_RelativeDirectionOfWindAndTree.push_back(tempDirections);
}

CWindField::CWindField(const glm::vec3 vWindCenter, const int vSize, const std::vector<SWaveFunctionPara> vSwavePara, const int vWavelength, double Sphere4Influence, std::vector<glm::vec3>& vWindCenterMoveVelocity, std::vector<int>& vWindCenterMoveFrames, std::vector<float> & vMoveScale)
{
    m_WindCenter = vWindCenter;
    m_WindInfluenceField = Sphere4Influence;
    m_WindSourceCenterMoveVelocity = vWindCenterMoveVelocity;
    m_WindCenterMoveFrames = vWindCenterMoveFrames;
    m_WindMoveScale = vMoveScale;
    std::vector<int>tempForces(vSize, 0);
    for (auto tempwavepara : vSwavePara)
    {
        std::vector<int> temp = GenerateSamplingForce(vSize, tempwavepara.Amplitude, tempwavepara.Frequency, tempwavepara.Phase, tempwavepara.Ypluse, vWavelength);
        if (tempwavepara.Pulse == 1)
        {
            for (int i = 0; i < vSize; i++)
            {
                if (temp[i] < tempwavepara.Ypluse)
                {
                    temp[i] = tempwavepara.Ypluse;
                    if (tempwavepara.DropNumber != 0)
                    {
                        temp[i] -= tempwavepara.DropNumber;
                    }
                }
            }
        }
        for (int i = 0; i < vSize; i++)
        {
            tempForces[i] += temp[i];
        }
    }
    m_WindCenterForces = tempForces;

    if (m_WindSourceCenterMoveVelocity.size() != m_WindCenterMoveFrames.size())
        std::cout << "the WindSourceMoveVelocity Size must same as WindCenterMoveFrames" << std::endl;

    int tempFrames=0;
    for (auto i : m_WindCenterMoveFrames)
        tempFrames += i;
    if (tempFrames != Common::ProductFrameNumber)
        std::cout << "the WindCenterMoveFrames must same as Common::ProductFrameNumber" << std::endl;

    glm::vec3 tempWindSourcePosition= m_WindCenter;
    for (int i = 0; i < m_WindSourceCenterMoveVelocity.size(); i++)
    {
        glm::vec3 tempVelocity = m_WindSourceCenterMoveVelocity[i];
        for (int k = 0; k < m_WindCenterMoveFrames[i]; k++)
        {
            tempWindSourcePosition += glm::vec1((k + 1))*tempVelocity*glm::vec1(m_WindMoveScale[i]);
            m_WindSourceCenterMovePosition.push_back(tempWindSourcePosition);
        }
    }

    m_EachTypeTreeNumberIndexArea.push_back(0);

}
void CWindField::setEachTypeTreeNumber(int vTreeNumber)
{
    int tempNumber = 0;
    for (auto i : m_EachTypeTreeNumberIndexArea)
        tempNumber += i;
    tempNumber += vTreeNumber;
    m_EachTypeTreeNumberIndexArea.push_back(tempNumber);
}

void CWindField::setTreePositionAndRotationAngle(std::vector<glm::vec3>& vTreePosition, std::vector<Common::SForceDirection>& vTreeRotationDirection)
{
    m_TreesPosition.push_back(vTreePosition);
    m_TreesRotationAngle.push_back(vTreeRotationDirection);
}

void CWindField::caculateWindForcesAndDirection2Trees()
{
    for (int TreeTypeIndex = 0; TreeTypeIndex < m_TreesPosition.size(); TreeTypeIndex++)
    {
        for (int TreeIndexInType = 0; TreeIndexInType < m_TreesPosition[TreeTypeIndex].size(); TreeIndexInType++)
            caculateWindForcesAndDirection2OneTree(TreeTypeIndex, TreeIndexInType);
    }
}

void CWindField::caculateWindForcesAndDirection2OneTree(int vTreeType,int vTreeIndex)
{
    std::vector<int> oneTreeForces;
    std::vector<Common::SForceDirection> oneTreeRelativeDirection;
    for (int i = 0; i < m_WindSourceCenterMovePosition.size(); i++)
    {
        double Radius;
        int Theta=0, Phi=0;
        
        TransformCartesianCorrdinate2SphericalCorrdinate(m_WindSourceCenterMovePosition[i], m_TreesPosition[vTreeType][vTreeIndex], Radius, Theta, Phi);
        //////
        //std::cout <<"Theta"<< Theta <<"Phi"<< Phi << std::endl;
        //生成0-360
        Common::SForceDirection tempWindDirection(Theta, Phi);
        Common::SForceDirection tempDirection = caculateRelativeDirection(tempWindDirection, m_TreesRotationAngle[vTreeType][vTreeIndex]);
        oneTreeRelativeDirection.push_back(tempDirection);
        oneTreeForces.push_back(__caculateWindForceOnTree(i, m_TreesPosition[vTreeType][vTreeIndex]));
    }
    m_Forces.push_back(oneTreeForces);
    m_RelativeDirectionOfWindAndTree.push_back(oneTreeRelativeDirection);
}

void CWindField::getSpecificWindForcesAndDirection(int vTreeType, std::vector<std::vector<int>> &voForcesInSameType, std::vector<std::vector<Common::SForceDirection>>& voDirectionInSameType)
{
    for (int i = m_EachTypeTreeNumberIndexArea[vTreeType]; i < m_EachTypeTreeNumberIndexArea[vTreeType+1]; i++)
    {
        voForcesInSameType.push_back(m_Forces[i]);
        voDirectionInSameType.push_back(m_RelativeDirectionOfWindAndTree[i]);
    }
}

int CWindField::__caculateWindForceOnTree(int vFrameIndex, glm::vec3& vTreePosition)
{

    glm::vec3 RelativeDistance=m_WindSourceCenterMovePosition[vFrameIndex]-vTreePosition;
    double absRelativeDistance = sqrt(RelativeDistance.x*RelativeDistance.x + RelativeDistance.z*RelativeDistance.z);

    //y=-(1/N)*x+1
    //int Force = m_WindCenterForces[vFrameIndex] * (-(absRelativeDistance / m_WindInfluenceField) + 1);
    /////////////////////
    int Force = m_WindCenterForces[vFrameIndex];

    return Force;
}

Common::SForceDirection CWindField::caculateRelativeDirection(Common::SForceDirection &vWindDirection, Common::SForceDirection &vTreeRotationDirection)
{
    Common::SForceDirection tempDirection(0, 0);
    tempDirection.Phi = vWindDirection.Phi - vTreeRotationDirection.Phi;
    
   // std::cout << -tempDirection.Phi;
    //tempDirection.Phi = (-tempDirection.Phi);
    //////////////////这里修改
    if (tempDirection.Phi < 0)
    {
        tempDirection.Phi += 360;
    }
    return tempDirection;
}

void CWindField::saveForces2File(const std::string filePath)
{
    std::ofstream outFile;
    outFile.open(filePath, std::ios::in | std::ios::app);
    if (!outFile)
    {
        std::cout << "Can't open the file!!!" << std::endl;
        return;
    }
  
    for (int i = 0; i < m_Forces.size(); i++)
    {
        outFile << "/////////////////////" << std::endl;
        for (int k = 0; k < m_Forces[i].size(); k++)
        {
            outFile << m_Forces[i][k] << ", ";
            if (k % 5 == 0&&k!=0)
            {
                outFile << "\n";
            }
        }
    }
    outFile.close();
}
//
//void CWindField::setWindSourceRendering()
//{
//    for (int i = 0; i < 6; i++)
//    {
//        renderPoints.push_back()
//    }
//    glGenVertexArrays(1, &m_WindVAO);
//    glGenBuffers(1, &m_WindVBO);
//    glBindVertexArray(m_WindVAO);
//    glBindBuffer(GL_ARRAY_BUFFER, m_WindVBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(lightPositionsData), &lightPositionsData, GL_STATIC_DRAW);
//}
