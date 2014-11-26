#include "J2716SimulationDataGenerator.h"
#include "J2716AnalyzerSettings.h"

#include <AnalyzerHelpers.h>

J2716SimulationDataGenerator::J2716SimulationDataGenerator()
:	mSerialText( "My first analyzer, woo hoo!" ),
	u32MicroSecondVal( 0 )
{
}

J2716SimulationDataGenerator::~J2716SimulationDataGenerator()
{
}

void J2716SimulationDataGenerator::Initialize( U32 simulation_sample_rate, J2716AnalyzerSettings* settings )
{
	mSimulationSampleRateHz = simulation_sample_rate;
	mSettings = settings;

	mSerialSimulationData.SetChannel( mSettings->mInputChannel );
	mSerialSimulationData.SetSampleRate( simulation_sample_rate );
	mSerialSimulationData.SetInitialBitState( BIT_HIGH );
}

U32 J2716SimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel )
{
	U8 u8SampleIndex = 0;
	U8 au8DataSamples[ 8/*Rows*/ ][6/*Cols*/] = {	{ 0x4 , 0x7 , 0x8 , 0x3 , 0x4 , 0x7 }/*CRC=0xD*/,
													{ 0x4 , 0x7 , 0x8 , 0x3 , 0x4 , 0x3 }/*CRC=0x3*/,
													{ 0x4 , 0x7 , 0x8 , 0x3 , 0x4 , 0x5 }/*CRC=0xA*/,
													{ 0x4 , 0x7 , 0x8 , 0x3 , 0x4 , 0x4 }/*CRC=0x7*/,
													{ 0x4 , 0x7 , 0x7 , 0x3 , 0x4 , 0x5 }/*CRC=0xC*/,
													{ 0x4 , 0x7 , 0x9 , 0x3 , 0x4 , 0x5 }/*CRC=0xE*/,
													{ 0x4 , 0x7 , 0x9 , 0x3 , 0x4 , 0x6 }/*CRC=0x4*/,
													{ 0x4 , 0x7 , 0x7 , 0x3 , 0x4 , 0x7 }/*CRC=0xB*/};
	U32 u32SampleData = 0x00DA4256;
	const U32 ku32MegaHert = 1000000U;
	const U32 ku32MicroSecond = mSimulationSampleRateHz / ku32MegaHert;
	U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );
	u32MicroSecondVal = ku32MicroSecond;

	while( mSerialSimulationData.GetCurrentSampleNumber() < adjusted_largest_sample_requested )
	{
		CreateSyncField();
		StartOfData();
		/*ToDo: Generate STATUS NUBBLE*/
		CreateSerialNibbles( u32SampleData );
		if( true == mSettings->boPauseFlg )
		{
			CreatePausePulse();
		}
		else
		{
		}
		u8CrcVal = ComputeCrc( &au8DataSamples[ u8SampleIndex ][ 0 ] , 6 );
		StartOfData( );				/*Start CRC*/
		AdvanceTicks( u8CrcVal );	/*Add CRC*/

		u32SampleData += 5;/*Change the simulated value*/
		
		u8SampleIndex++;		/*Point to next sample buffer*/
		u8SampleIndex &= 0x7;	/*Mask index (in oder to select from 0 to 7)*/
	}

	*simulation_channel = &mSerialSimulationData;
	return 1;
}

void J2716SimulationDataGenerator::CreateSerialNibbles( U32 u32SampleData )
{
	U8 u8TempNibbleVal=0;
	U32 u32Temp = u32SampleData;
	//const U8 kau8CrcLookupTbl[ ] = { 0 , 13 , 7 , 10 , 14 , 3 , 9 , 4 , 1 , 12 , 6 , 11 , 15 , 2 , 8 , 5 };
	//u8CrcVal = ku8CrcSeed;
	for( U16 i = 0; i < mSettings->u32Nibbles; i++ )
	{
		//u8TempNibbleVal = (U8)(u32SampleData & 0x0F);/* Mask nibble */
		StartOfData();
		AdvanceTicks(u8TempNibbleVal);
		//u8CrcVal ^= kau8CrcLookupTbl[ u8CrcVal ];
		u32SampleData>>=4;/*Shift one nibble*/
	}
}

void J2716SimulationDataGenerator::CreateSyncField( )
{
	CreateLowPulse();
	AdvanceTicks( 51 );
}

void J2716SimulationDataGenerator::CreateLowPulse()
{
	mSerialSimulationData.TransitionIfNeeded( BIT_LOW );
	AdvanceTicks( ku8LowPulseTicks );
	mSerialSimulationData.TransitionIfNeeded( BIT_HIGH );
}

void J2716SimulationDataGenerator::CreatePausePulse( )
{
	CreateLowPulse();
	AdvanceTicks( 72 );
}

void J2716SimulationDataGenerator::AdvanceTicks( U8 u8Ticks )
{
	mSerialSimulationData.Advance( ( mSettings->mBitRate* u32MicroSecondVal ) * u8Ticks );
}

void J2716SimulationDataGenerator::StartOfData( )
{
	CreateLowPulse();
	AdvanceTicks( 7 );
}

U8 J2716SimulationDataGenerator::ComputeCrc( U8* pu8Data , U8 u8Len )
{
	const U8 kau8CrcLookupTbl[ 16 ] = { 0 , 13 , 7 , 10 , 14 , 3 , 9 , 4 , 1 , 12 , 6 , 11 , 15 , 2 , 8 , 5 };
	U8 u8Crc = ku8CrcSeed;/*Seed*/
	U8 u8Temp;
	for( U8 i = 0; i < u8Len; i++ )
	{
		u8Temp = kau8CrcLookupTbl[u8Crc];
		u8Crc = u8Temp ^  *pu8Data;
		//u8Crc = kau8CrcLookupTbl[ ( u8Crc ^ ( *pu8Data ) ) & 0x0FU ];
		pu8Data++;
		printf( "%d" , u8Crc );
	}
	u8Crc = kau8CrcLookupTbl[ u8Crc ];
	return u8Crc;
}