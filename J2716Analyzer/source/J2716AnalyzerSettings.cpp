#include "J2716AnalyzerSettings.h"
#include <AnalyzerHelpers.h>


J2716AnalyzerSettings::J2716AnalyzerSettings()
: mInputChannel( UNDEFINED_CHANNEL ) ,
	mBitRate( 3 ),
	mNibblesToSendInterface(),
	mPauseInterface( false ),
	mDecodePhysValInterface(false)
{
	/* Select Saleae's logic channel */
	mInputChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mInputChannelInterface->SetTitleAndTooltip( "J2716 Data", "J2716 Unidirectional Data Line" );
	mInputChannelInterface->SetChannel( mInputChannel );

	/*  */
	mBitRateInterface.reset( new AnalyzerSettingInterfaceInteger() );
	mBitRateInterface->SetTitleAndTooltip( "TICK (µs)",  "Specify the \"Unit Time\" (UT) or \"TICK\" in microseconds" );
	mBitRateInterface->SetMax( 90 );
	mBitRateInterface->SetMin( 3 );
	mBitRateInterface->SetInteger( mBitRate );

	/* Nummer of nibbles to be decoded */
	mNibblesToSendInterface.reset( new AnalyzerSettingInterfaceNumberList() );
	mNibblesToSendInterface->SetTitleAndTooltip( "Nibles Decoded" , "The number of nibbles to decode on the J2716 Data Line" );
	mNibblesToSendInterface->AddNumber( 1 , "1 Nibble Msg" , "Data to be decoded" );
	mNibblesToSendInterface->AddNumber( 2 , "2 Nibble Msg" , "Data to be decoded" );
	mNibblesToSendInterface->AddNumber( 3 , "3 Nibble Msg" , "Data to be decoded" );
	mNibblesToSendInterface->AddNumber( 4 , "4 Nibble Msg" , "Data to be decoded" );
	mNibblesToSendInterface->AddNumber( 5 , "5 Nibble Msg" , "Data to be decoded" );
	mNibblesToSendInterface->AddNumber( 6 , "6 Nibble Msg" , "Data to be decoded" );
	mNibblesToSendInterface->SetNumber( 6 );/*MaLo: There must be at least one selected, otherwise it crashes*/

	mPauseInterface.reset( new AnalyzerSettingInterfaceBool() );
	mPauseInterface->SetTitleAndTooltip( "Enable Pause" , "Enable \"pause\" decoding" );
	mPauseInterface->SetCheckBoxText( "Enable \"pause\" decoding after the data nibbles" );
	mPauseInterface->SetValue( false );

	mDecodePhysValInterface.reset( new AnalyzerSettingInterfaceBool( ) );
	mDecodePhysValInterface->SetTitleAndTooltip( "Physical Value" , "Enable \"physical value\" decoding\ni.e. 0x502=12.65V" );
	mDecodePhysValInterface->SetCheckBoxText( "Enable \"Physical Value\" decoding\ni.e. 0x502=12.65V" );
	mDecodePhysValInterface->SetValue( false );

	/* Expose interfaces */
	AddInterface( mInputChannelInterface.get() );
	AddInterface( mBitRateInterface.get() );
	AddInterface( mNibblesToSendInterface.get( ) );
	AddInterface( mPauseInterface.get() );
	AddInterface( mDecodePhysValInterface.get( ) );
	
	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
	AddChannel( mInputChannel, "J2716 (SENT)", false );
}

J2716AnalyzerSettings::~J2716AnalyzerSettings()
{
}

bool J2716AnalyzerSettings::SetSettingsFromInterfaces()
{	/* Get values from "Settings" GUI and stored them in their corresponding valirables */
	mInputChannel = mInputChannelInterface->GetChannel();
	mBitRate = mBitRateInterface->GetInteger();
	u32Nibbles = (U8)mNibblesToSendInterface->GetNumber();
	boPauseFlg = mPauseInterface->GetValue();
	boPhysicalValueFlg = mDecodePhysValInterface->GetValue();

	ClearChannels();
	AddChannel( mInputChannel, "J2716 (SENT)", true );

	return true;
}

void J2716AnalyzerSettings::UpdateInterfacesFromSettings()
{	/* Set the values from stored variables to the "Settings" GUI */
	mInputChannelInterface->SetChannel( mInputChannel );
	mBitRateInterface->SetInteger( mBitRate );
	mNibblesToSendInterface->SetNumber((double)u32Nibbles);
	mPauseInterface->SetValue( boPauseFlg );
	mDecodePhysValInterface->SetValue( boPhysicalValueFlg );
}

void J2716AnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );

	text_archive >> mInputChannel;
	text_archive >> mBitRate;
	text_archive >> u32Nibbles;
	text_archive >> boPauseFlg;
	text_archive >> boPhysicalValueFlg;

	ClearChannels();
	AddChannel( mInputChannel, "J2716 (SENT)", true );

	UpdateInterfacesFromSettings();
}

const char* J2716AnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << mInputChannel;
	text_archive << mBitRate;
	text_archive << u32Nibbles;
	text_archive << boPauseFlg;
	text_archive << boPhysicalValueFlg;

	return SetReturnString( text_archive.GetString() );
}
