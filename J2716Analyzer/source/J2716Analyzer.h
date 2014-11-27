#ifndef J2716_ANALYZER_H
#define J2716_ANALYZER_H

#include <Analyzer.h>
#include "J2716AnalyzerResults.h"
#include "J2716SimulationDataGenerator.h"

/* Possible valus for "SENT_DBG" */
#define SENT_DBG_OFF	(0)
#define SENT_DBG_ON		(1)

/* Switch for debugging */
#define	SENT_DBG	SENT_DBG_ON

#define	SENT_FLAG1		((U8)0x01)
#define	SENT_FLAG2		((U8)0x02)
#define	SENT_FLAG3		((U8)0x04)
#define	SENT_FLAG4		((U8)0x08)
#define	SENT_FLAG5		((U8)0x01)
#define	SENT_FLAG6		((U8)0x02)
/*							 0x40	RESERVED by "DISPLAY_AS_WARNING_FLAG" in <AnalyzerResults.h> */
/*							 0x80	RESERVED by "DISPLAY_AS_ERROR_FLAG" in <AnalyzerResults.h> */

/* Types of SENT packets */
enum J2716Packet
{
	SENTSync , SENTStatus , SENTData , SENTCrc , SENTPause , SENTError
};

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
	AnalyzerChannelData* mJ2716Data;

	J2716SimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;

	//Serial analysis vars:
	U32 mSampleRateHz;
	U32 mStartOfStopBitOffset;
	U32 mEndOfStopBitOffset;
private:
	const U8 ku8SyncTicks = 56;
	const U8 ku8PauseTicks = 77;
	const U8 ku8MinTicks = 12;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //J2716_ANALYZER_H
