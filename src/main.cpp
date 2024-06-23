#include <Geode/modify/LevelInfoLayer.hpp>
#include "cvoltonLevelTime.cpp"
#include <cmath>
#include <thread>

using namespace geode::prelude;

std::string createXLstring(int levelLengthMinutes) {
	std::stringstream XLstring;
	int maximumXs = Mod::get()->getSettingValue<int64_t>("maximum-xs");
	bool usingPowerNotation = Mod::get()->getSettingValue<bool>("use-power-notation");
	bool usingXXLplus = Mod::get()->getSettingValue<bool>("xxl-plus");
	int lengthExponent = log2(levelLengthMinutes);

	if (usingPowerNotation && lengthExponent > maximumXs) {
		XLstring << "X^" << std::to_string(lengthExponent);
	}

	else {
		if (lengthExponent > maximumXs) {
			lengthExponent = maximumXs;
		}
		for (int i = 0; i < lengthExponent; i++) {
			XLstring << "X";
		}
	}
	XLstring << "L";

	if (!usingXXLplus) {
		return XLstring.str();
	}

    int XXLPlusLength = pow(2, lengthExponent) + pow(2, lengthExponent - 1);
    if (levelLengthMinutes >= XXLPlusLength) {
		XLstring << "+";
    }

    return XLstring.str();
};

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
	void modifyXLlabel(int levelLengthMinutes) {
		bool usingRed = Mod::get()->getSettingValue<bool>("use-red");
		m_lengthLabel->setString(createXLstring(levelLengthMinutes).c_str());
		int maximumRed = Mod::get()->getSettingValue<int64_t>("maximum-red");
		if (usingRed) {
			GLubyte redGradient=levelLengthMinutes<maximumRed ? 255-levelLengthMinutes*255/maximumRed : 0; 
			m_lengthLabel->setColor({255,redGradient,redGradient});
		}
	}

	// Using CVolton's time calculator for levels before 2.2
	void createXLlabelCvolton() {
		this->retain();
		
		std::thread([this](){
			thread::setName("CVoltonTime");
			int cvoltonLengthMinutes = timeForLevelString(m_level->m_levelString);

			Loader::get()->queueInMainThread([this,cvoltonLengthMinutes]() {
				if (cvoltonLengthMinutes >= 3) {
					modifyXLlabel(cvoltonLengthMinutes);
				}
				this->release();
			});
		}).detach();
	}

	void createXLlabel() {
		if (m_level->isPlatformer()) {
			return;
		}
		
		int levelLengthMinutes = m_level->m_timestamp / 14400;
		
		
		if (levelLengthMinutes<=0) {
			createXLlabelCvolton();
			return;
		}

		if (levelLengthMinutes < 3) {
			return;
		}

		modifyXLlabel(levelLengthMinutes);
	}

	void setupLevelInfo() {
		LevelInfoLayer::setupLevelInfo();
		MyLevelInfoLayer::createXLlabel();
	}

	virtual void levelDownloadFinished(GJGameLevel* p0) {
		LevelInfoLayer::levelDownloadFinished(p0);
		MyLevelInfoLayer::createXLlabel();
	}
};