#ifndef J2716_ANALYZER_RESULTS
#define J2716_ANALYZER_RESULTS

#include <AnalyzerResults.h>

class J2716Analyzer;
class J2716AnalyzerSettings;

class J2716AnalyzerResults : public AnalyzerResults
{
public:
	J2716AnalyzerResults( J2716Analyzer* analyzer, J2716AnalyzerSettings* settings );
	virtual ~J2716AnalyzerResults();

	virtual void GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base );
	virtual void GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id );

	virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base );
	virtual void GeneratePacketTabularText( U64 packet_id, DisplayBase display_base );
	virtual void GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base );

protected: //functions

protected:  //vars
	J2716AnalyzerSettings* mSettings;
	J2716Analyzer* mAnalyzer;
};

#endif //J2716_ANALYZER_RESULTS
