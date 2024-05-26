#include <Geode/modify/LevelInfoLayer.hpp>
#include "cvoltonLevelTime.cpp"
#include <cmath>
#include <thread>

using namespace geode::prelude;

std::string createXLstring(int levelLengthMinutes) {
	std::string XLstring;
	int maximumXs = Mod::get()->getSettingValue<int64_t>("maximum-xs");
	bool usingPowerNotation = Mod::get()->getSettingValue<bool>("use-power-notation");
	bool usingXXLplus = Mod::get()->getSettingValue<bool>("xxl-plus");
	int lengthExponent = log2(levelLengthMinutes);

	if (usingPowerNotation && lengthExponent > 1) {
		XLstring.append("X^");
		XLstring.append(std::to_string(lengthExponent));
	}

	else {
		if (lengthExponent > maximumXs) {
			lengthExponent = maximumXs;
		}
		for (int i = 0; i < lengthExponent; i++) {
			XLstring.append("X");
		}
	}
	XLstring.append("L");

	if (!usingXXLplus) {
		return XLstring;
	}

    int XXLPlusLength = pow(2, lengthExponent) + pow(2, lengthExponent - 1);
    if (levelLengthMinutes >= XXLPlusLength) {
		XLstring.append("+");
    }

    return XLstring;
};

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
	// Using CVolton's time calculator for levels before 2.2
	void createXLlabelCvolton() {
		this->retain();
		
		std::thread([this](){
			thread::setName("CVoltonTime");
			int cvoltonLengthMinutes = timeForLevelString(m_level->m_levelString);

			Loader::get()->queueInMainThread([this,cvoltonLengthMinutes]() {
				if (cvoltonLengthMinutes >= 3) {
					m_lengthLabel->setString(createXLstring(cvoltonLengthMinutes).c_str());
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

		m_lengthLabel->setString(createXLstring(levelLengthMinutes).c_str());
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