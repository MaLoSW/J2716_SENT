#include "J2716Analyzer.h"
#include "J2716AnalyzerSettings.h"
#include <AnalyzerChannelData.h>

J2716Analyzer::J2716Analyzer()
:	Analyzer(),  
	mSettings( new J2716AnalyzerSettings() ),
	mSimulationInitilized( false )
{
	SetAnalyzerSettings( mSettings.get() );
}

J2716Analyzer::~J2716Analyzer()
{
	KillThread();
}

void J2716Analyzer::WorkerThread()
{
	U8 u8Data = 0;
	U8 u8J2716Val = 0;
	U8 u8LowTickCtr = 0;
	U8 u8HighTicksCtr = 0;
	bool boJ2716SyncFlag = false;
	bool boJ2716StatusFlag = false;
	bool boJ2716DataFlag = false;
	bool boJ2716CrcFlag = false;
	bool boJ2716PauseFlag = false;
	const U32 ku32MegaHert = 1000000U;
	U32 ku32MicroSecond;
	
	const double kdMicro = 1E-6;
	const double kdTick = double( mSettings->mBitRate )*kdMicro;
	const double kdSyncPeriod = ku8SyncTicks * kdTick;
	const double kdPausePeriod = ku8PauseTicks * kdTick;
	const double kdMinPeriod = ku8MinTicks * kdTick;
	double dPeriod = double( 0 );
	double dPeriodMin = double( 0 );
	double dPeriodMax = double( 0 );

	mResults.reset( new J2716AnalyzerResults( this, mSettings.get() ) );
	SetAnalyzerResults( mResults.get() );
	mResults->AddChannelBubblesWillAppearOn( mSettings->mInputChannel );/*Set where the results will be displayed on*/

	mSampleRateHz = GetSampleRate( );/*Get Current samplig rate in Hertz (i.e. 24000000 = 24 MHz)*/

	mJ2716Data = GetAnalyzerChannelData( mSettings->mInputChannel );

	if( mJ2716Data->GetBitState() == BIT_LOW )
		mJ2716Data->AdvanceToNextEdge();
	
	ku32MicroSecond = U32( mSampleRateHz ) / U32( ku32MegaHert );
	U32 u32Tick = mSettings->mBitRate*ku32MicroSecond;
	U32 u32HalfTick = u32Tick / 2;
	//U32 samples_per_bit = mSampleRateHz / mSettings->mBitRate;
	//U32 samples_to_first_center_of_first_data_bit = U32( 1.5 * double( mSampleRateHz ) / double( mSettings->mBitRate ) );

	for( ; ; )
	{
		Frame frmSync;
		Frame frmData;

		if( mJ2716Data->GetBitState() == BIT_HIGH )
		{
			mJ2716Data->AdvanceToNextEdge(); //falling edge -- beginning of the start bit
			
		}

		U64 u64StartingSample = mJ2716Data->GetSampleNumber();		/*START of packet */
		U64 u64RisingEdgeSample=mJ2716Data->GetSampleOfNextEdge();	/*NEXT rising edge*/
		U64 u64EndOfData = 0;
		U64 u64Temp = 0;


		/* Allow correct marker when Pause decoding is disabled*/
		if( mSettings->boPauseFlg == false )
		{
			boJ2716PauseFlag = true;
		}
		/*Start of data*/
		frmSync.mStartingSampleInclusive = u64StartingSample;
		if( false == boJ2716SyncFlag && 
			false == boJ2716StatusFlag &&
			true == boJ2716DataFlag &&
			true == boJ2716CrcFlag &&
			true == boJ2716PauseFlag )
		{
			boJ2716SyncFlag = true;
			mResults->AddMarker( mJ2716Data->GetSampleNumber() , mResults->Start , mSettings->mInputChannel );
		}


		u8LowTickCtr = 0;
		u8HighTicksCtr = 0;

		while( mJ2716Data->GetSampleNumber() < u64RisingEdgeSample )
		{
			mJ2716Data->Advance( u32Tick );
#if SENT_DBG==SENT_DBG_ON
			if( mJ2716Data->GetSampleNumber() < u64RisingEdgeSample )
			{
				mResults->AddMarker( mJ2716Data->GetSampleNumber() , mResults->Dot , mSettings->mInputChannel );/*Debug ONLY*/
			}
			else
			{
				mResults->AddMarker( mJ2716Data->GetSampleNumber( ) , mResults->UpArrow , mSettings->mInputChannel );/*Debug ONLY*/
			}
#endif
			u8LowTickCtr++;
		}

		if( mJ2716Data->GetBitState() == BIT_HIGH )
		{
			mJ2716Data->AdvanceToNextEdge( );				/*FALLING edge*/
		}
		frmSync.mData2 = u64RisingEdgeSample;
		u64EndOfData = mJ2716Data->GetSampleNumber( );
		frmSync.mEndingSampleInclusive = u64EndOfData;
		dPeriod = ( double( frmSync.mEndingSampleInclusive ) - double( frmSync.mStartingSampleInclusive ) ) / double( mSampleRateHz );

		/*Identify SENT packet here*/
		if( dPeriod >= kdMinPeriod )
		{
			dPeriodMax = kdSyncPeriod*1.2;/*+20%*/
			dPeriodMin = kdSyncPeriod*0.8;/*-20%*/
			if( ( dPeriod > dPeriodMin ) && ( dPeriod < dPeriodMax ) )
			{
				frmSync.mType = SENTSync;
			}
			else
			{
				dPeriodMax = kdPausePeriod*1.2;/*+20%*/
				dPeriodMin = kdPausePeriod*0.8;/*-20%*/
				if( ( dPeriod > dPeriodMin ) && ( dPeriod < dPeriodMax ) )
				{
					frmSync.mType = SENTPause;
					boJ2716PauseFlag = true;
				}
				else
				{
					/*MaLo: Decode data here*/
					u64Temp = u64RisingEdgeSample;
					while( u64Temp < u64EndOfData )
					{
						u64Temp += u32Tick;
						u8HighTicksCtr++;
					}
					u8J2716Val = ( u8LowTickCtr + u8HighTicksCtr ) - ku8MinTicks;
					/*Passes the value decoded*/
					frmSync.mData1 = u8J2716Val;

					/*Indentify the type of data */
					if( false == boJ2716StatusFlag )
					{
						boJ2716StatusFlag = true;
						frmSync.mType = SENTStatus;
					}
					else
					{	/* Receivind data  */
						if( u8Data < mSettings->u32Nibbles )
						{
							boJ2716DataFlag = true;
							frmSync.mType = SENTData;
							frmSync.mData2 = u8Data;
							au8InternalBuffer[ u8Data ] = frmSync.mData1;
							u8Data++;
						}
						else
						{
							boJ2716CrcFlag = true;
							frmSync.mType = SENTCrc;
							frmSync.mData2 = u8CalcCrc( au8InternalBuffer , mSettings->u32Nibbles );
							/*J2716 Frame ends here*/
							//mResults->AddMarker( mJ2716Data->GetSampleNumber() , mResults->Stop , mSettings->mInputChannel );
							u8Data = 0;
							boJ2716PauseFlag = false;
							boJ2716StatusFlag = false;
							boJ2716SyncFlag = false;
						}
					}
				}
			}
		}
		else
		{
			frmSync.mType = SENTError;
		}

		/*Store frame*/
		mResults->AddFrame( frmSync );
		mResults->CommitResults();
		ReportProgress( frmSync.mEndingSampleInclusive );
	}
}

bool J2716Analyzer::NeedsRerun()
{
	return false;
}

U32 J2716Analyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), mSettings.get() );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 J2716Analyzer::GetMinimumSampleRateHz()
{
	return mSettings->mBitRate * 4;
}

const char* J2716Analyzer::GetAnalyzerName() const
{
	return "J2716";/*Name displayed in "Add Analyzer" drop-down*/
}

const char* GetAnalyzerName()
{
	return "J2716";
}

Analyzer* CreateAnalyzer()
{
	return new J2716Analyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}

U8 J2716Analyzer::u8CalcCrc( U8* pu8Data , U8 u8Len )
{
	const U8 kau8CrcLookupTbl[ 16 ] = { 0 , 13 , 7 , 10 , 14 , 3 , 9 , 4 , 1 , 12 , 6 , 11 , 15 , 2 , 8 , 5 };
	U8 u8Crc = ku8CrcSeed;/*Seed*/
	U8 u8Temp;
	for( U8 i = 0; i < u8Len; i++ )
	{
		u8Temp = kau8CrcLookupTbl[ u8Crc ];
		u8Crc = u8Temp ^  *pu8Data;
		pu8Data++;
	}
	u8Crc = kau8CrcLookupTbl[ u8Crc ];
	return u8Crc;
}