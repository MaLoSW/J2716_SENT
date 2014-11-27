#include "J2716AnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "J2716Analyzer.h"
#include "J2716AnalyzerSettings.h"
#include <iostream>
#include <fstream>

J2716AnalyzerResults::J2716AnalyzerResults( J2716Analyzer* analyzer, J2716AnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

J2716AnalyzerResults::~J2716AnalyzerResults()
{
}

void J2716AnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{
	ClearResultStrings();
	Frame frame = GetFrame( frame_index );
	char i8StringData1[ 64 ];
	char i8StringData2[ 64 ];
	char i8StringTime[ 16 ];

	AnalyzerHelpers::GetNumberString( frame.mData1 , display_base , 2 , &i8StringData1[ 0 ] , sizeof( i8StringData1 ) );
	switch( frame.mType )
	{
		case SENTSync:
		AnalyzerHelpers::GetTimeString( frame.mEndingSampleInclusive , frame.mStartingSampleInclusive , mAnalyzer->GetSampleRate( ) , &i8StringTime[ 0 ] , 9/* x.xxxxxx */ );
		AddResultString( "SYNC " );
		AddResultString( "SYNC Field " );
		AddResultString( "SYNC Field [" , i8StringTime , " s]" );
		break;
		case SENTStatus:
		AddResultString( "STAT " );
		AddResultString( "STATUS " );
		AddResultString( "STATUS " , i8StringData1 );
		/*ToDo: Process SENT Status*/
		break;
		case SENTData:
		AnalyzerHelpers::GetNumberString( frame.mData2 , Decimal , 8 , &i8StringData2[ 0 ] , sizeof( i8StringData2 ) );
		AddResultString( "DATA " );
		AddResultString( "DATA " , i8StringData2 );
		AddResultString( "DATA " , i8StringData2 , " [" , i8StringData1 , "]" );
		/*ToDo: Process SENT Data*/
		break;
		case SENTCrc:
		AddResultString( "CRC " );
		AddResultString( "CRC " , " [" , i8StringData1 , "]" );
		/*ToDo: Process SENT CRC*/
		break;
		case SENTPause:
		AddResultString( "PAUSE " );
		/*ToDo: Process SENT Pause*/
		break;
		default:
		//frame.mFlags |= DISPLAY_AS_ERROR_FLAG;
		AnalyzerHelpers::GetNumberString( frame.mData2 , Decimal , 8 , &i8StringData2[ 0 ] , sizeof( i8StringData2 ) );
		AnalyzerHelpers::GetTimeString( frame.mEndingSampleInclusive , frame.mStartingSampleInclusive , mAnalyzer->GetSampleRate( ) , &i8StringTime[ 0 ] , 9/* x.xxxxxx */ );
		AddResultString( "?? " );
		AddResultString( "?? " , i8StringData2 );
		AddResultString( "?? " , i8StringData2 , " Ticks @ " , i8StringTime , " s" );
		break;
	}
}

void J2716AnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
{
	std::ofstream file_stream( file, std::ios::out );

	U64 trigger_sample = mAnalyzer->GetTriggerSample();
	U32 sample_rate = mAnalyzer->GetSampleRate();

	file_stream << "Time [s],Value" << std::endl;

	U64 num_frames = GetNumFrames();
	for( U32 i=0; i < num_frames; i++ )
	{
		Frame frame = GetFrame( i );
		
		char time_str[128];
		AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

		char number_str[128];
		AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );

		file_stream << time_str << "," << number_str << std::endl;

		if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
		{
			file_stream.close();
			return;
		}
	}

	file_stream.close();
}

void J2716AnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
	Frame frame = GetFrame( frame_index );
	ClearResultStrings();

	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );
	AddResultString( number_str );
}

void J2716AnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
	ClearResultStrings();
	AddResultString( "not supported" );
}

void J2716AnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
	ClearResultStrings();
	AddResultString( "not supported" );
}