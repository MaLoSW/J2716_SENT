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
		/*ToDo:Compute CRC*/
		u32SampleData += 5;/*Change the simulated value*/
	}

	*simulation_channel = &mSerialSimulationData;
	return 1;
}

void J2716SimulationDataGenerator::CreateSerialNibbles( U32 u32SampleData )
{
	U8 u8TempNibbleVal;
	U32 u32Temp = u32SampleData;
	
	for( U16 i = 0; i < mSettings->u32Nibbles; i++ )
	{
		u8TempNibbleVal = (U8)(u32SampleData & 0x0F);/* Mask nibble */
		StartOfData();
		AdvanceTicks(u8TempNibbleVal);
		u32SampleData>>=4;/*Shift one nibble*/
	}

#if 0
	//we're currenty high
	//let's move forward a little
	mSerialSimulationData.Advance( samples_per_bit * 10 );

	mSerialSimulationData.Transition();  //low-going edge for start bit
	mSerialSimulationData.Advance( samples_per_bit );  //add start bit time

	U8 mask = 0x1 << 7;
	for( U32 i=0; i<8; i++ )
	{
		if( ( byte & mask ) != 0 )
			mSerialSimulationData.TransitionIfNeeded( BIT_HIGH );
		else
			mSerialSimulationData.TransitionIfNeeded( BIT_LOW );

		mSerialSimulationData.Advance( samples_per_bit );
		mask = mask >> 1;
	}

	mSerialSimulationData.TransitionIfNeeded( BIT_HIGH ); //we need to end high

	//lets pad the end a bit for the stop bit:
	mSerialSimulationData.Advance( samples_per_bit );
#endif
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