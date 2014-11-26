#ifndef J2716_ANALYZER_SETTINGS
#define J2716_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class J2716AnalyzerSettings : public AnalyzerSettings
{
public:
	J2716AnalyzerSettings();
	virtual ~J2716AnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	
	Channel mInputChannel;
	U32		mBitRate;
	U32		u32Nibbles;
	bool	boPauseFlg;
	bool	boPhysicalValueFlg;
protected:
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mInputChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceInteger >	mBitRateInterface;
	std::auto_ptr<AnalyzerSettingInterfaceNumberList>	mNibblesToSendInterface;
	std::auto_ptr<AnalyzerSettingInterfaceBool>			mPauseInterface;
	std::auto_ptr<AnalyzerSettingInterfaceBool>			mDecodePhysValInterface;
};

#endif //J2716_ANALYZER_SETTINGS
