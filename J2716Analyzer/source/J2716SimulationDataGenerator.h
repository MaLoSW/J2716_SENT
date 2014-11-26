#ifndef J2716_SIMULATION_DATA_GENERATOR
#define J2716_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <string>
class J2716AnalyzerSettings;

class J2716SimulationDataGenerator
{
public:
	J2716SimulationDataGenerator();
	~J2716SimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, J2716AnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

protected:
	J2716AnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;

protected:
	void CreateSerialNibbles(U32 u32SampleData);
	void CreateSyncField();
	void CreateLowPulse();
	void CreatePausePulse();
	void AdvanceTicks(U8 u8Ticks );
	void StartOfData();
	std::string mSerialText;
	U32 u32MicroSecondVal;

	SimulationChannelDescriptor mSerialSimulationData;
private:
	const U8 ku8LowPulseTicks = 5;
};
#endif //J2716_SIMULATION_DATA_GENERATOR