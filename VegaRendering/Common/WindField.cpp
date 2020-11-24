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

Common::SForceDirection CWindField::caculateRelativeDirection(Common::SForceDirection &vWindDirection, Common::SForceDirection &vTreeRotationDirection)
{
    Common::SForceDirection tempDirection(0, 0);
    tempDirection.Phi = vWindDirection.Phi - vTreeRotationDirection.Phi;
    std::cout << tempDirection.Phi;
    std::cout << vWindDirection.Phi;
    std::cout << vWindDirection.Phi;
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
