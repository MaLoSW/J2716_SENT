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
	U8 u8TempCrc = 0;
	char ai8Data1Str[ 64 ];
	char ai8Data2Str[ 64 ];
	char ai8TicksStr[ 16 ];
	char ai8TimeStr[ 16 ];
	U8 u8Ticks = 0;
	const double kdMicro = 1E-6;
	const double kdRoundError = 1E-19;
	double dFramePeriod = double( 0 );

	dFramePeriod = ( double( frame.mEndingSampleInclusive ) - double( frame.mStartingSampleInclusive ) ) / double( mAnalyzer->GetSampleRate( ) ) + kdRoundError;
	u8Ticks = U8( dFramePeriod / double( mSettings->mBitRate*kdMicro ) );

	AnalyzerHelpers::GetNumberString( frame.mData1 , display_base , 2 , &ai8Data1Str[ 0 ] , sizeof( ai8Data1Str ) );
	AnalyzerHelpers::GetNumberString( u8Ticks , Decimal , 2 , &ai8TicksStr[ 0 ] , sizeof( ai8TicksStr ) );

	switch( frame.mType )
	{
		case SENTSync:
		AnalyzerHelpers::GetTimeString( frame.mEndingSampleInclusive , frame.mStartingSampleInclusive , mAnalyzer->GetSampleRate() , &ai8TimeStr[ 0 ] , 9/* x.xxxxxx + '\0'  */ );
		AddResultString( "SYNC " );
		AddResultString( "SYNC Field ~" , ai8TicksStr , " ticks" );
		AddResultString( "SYNC Field ~" , ai8TicksStr , " ticks [" , ai8TimeStr , " s]" );
		break;
		case SENTStatus:
		AddResultString( "STAT " );
		AddResultString( "STATUS " );
		AddResultString( "STATUS " , ai8Data1Str );
		AddResultString( "STATUS " , ai8Data1Str ," ~" , ai8TicksStr , " ticks" );
		break;
		case SENTData:
		AnalyzerHelpers::GetNumberString( frame.mData2 , Decimal , 8 , &ai8Data2Str[ 0 ] , sizeof( ai8Data2Str ) );
		AddResultString( "DATA " );
		AddResultString( "DATA " , ai8Data2Str );
		AddResultString( "DATA " , ai8Data2Str , " [" , ai8Data1Str , "]" );
		break;
		case SENTCrc:
		//u8TempCrc =mAnalyzer
		if( frame.mData1 == frame.mData2 )
		{
			AddResultString( "CRC " );
			AddResultString( "CRC OK" );
			AddResultString( "CRC OK [" , ai8Data1Str , "]" );
		}
		else
		{
			AnalyzerHelpers::GetNumberString( frame.mData2 , display_base , 2 , &ai8Data2Str[ 0 ] , sizeof( ai8Data2Str ) );
			AddResultString( "CRC! " );
			AddResultString( "CRC! Rx[" , ai8Data1Str , "] Calc{" , ai8Data2Str , "}" );
		}
		break;
		case SENTPause:
		AnalyzerHelpers::GetTimeString( frame.mEndingSampleInclusive , frame.mStartingSampleInclusive , mAnalyzer->GetSampleRate() , &ai8TimeStr[ 0 ] , 9/* x.xxxxxx */ );
		AddResultString( "PAUSE " );
		AddResultString( "PAUSE ~" , ai8TicksStr , " ticks" );
		AddResultString( "PAUSE ~" , ai8TicksStr , " ticks [",ai8TimeStr , " s]" );
		break;
		default:
		//frame.mFlags |= DISPLAY_AS_ERROR_FLAG;
		AnalyzerHelpers::GetNumberString( frame.mData2 , Decimal , 8 , &ai8Data2Str[ 0 ] , sizeof( ai8Data2Str ) );
		AnalyzerHelpers::GetTimeString( frame.mEndingSampleInclusive , frame.mStartingSampleInclusive , mAnalyzer->GetSampleRate() , &ai8TimeStr[ 0 ] , 9/* x.xxxxxx */ );
		AddResultString( "?? " );
		AddResultString( "?? " , ai8TicksStr );
		AddResultString( "?? " , ai8TicksStr , " Ticks @ " , ai8TimeStr , " s" );
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