#include"WindFiled.h"

CWindField::CWindField(const int vSize, const std::vector<SWaveFunctionPara> vSwavePara, const int vWavelength, Common::SWindDirection vWindDirection)
{
	std::vector<int>tempForces(vSize,0);
	for (auto tempwavepara : vSwavePara)
	{
		std::vector<int> temp=GenerateSamplingForce(vSize, tempwavepara.Amplitude, tempwavepara.Frequency, tempwavepara.Phase, tempwavepara.Ypluse, vWavelength);
		for (int i = 0; i < vSize; i++)
		{
			tempForces[i] += temp[i];
		}
	}
	std::vector<Common::SWindDirection> tempDirections(vSize, vWindDirection);
	m_Forces.push_back(tempForces);
	m_Directions.push_back(tempDirections);
}