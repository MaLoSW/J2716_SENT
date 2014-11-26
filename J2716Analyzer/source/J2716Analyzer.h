#ifndef J2716_ANALYZER_H
#define J2716_ANALYZER_H

#include <Analyzer.h>
#include "J2716AnalyzerResults.h"
#include "J2716SimulationDataGenerator.h"

class J2716AnalyzerSettings;
class ANALYZER_EXPORT J2716Analyzer : public Analyzer
{
public:
	J2716Analyzer();
	virtual ~J2716Analyzer();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();

protected: //vars
	std::auto_ptr< J2716AnalyzerSettings > mSettings;
	std::auto_ptr< J2716AnalyzerResults > mResults;
	AnalyzerChannelData* mSerial;

	J2716SimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;

	//Serial analysis vars:
	U32 mSampleRateHz;
	U32 mStartOfStopBitOffset;
	U32 mEndOfStopBitOffset;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //J2716_ANALYZER_H
