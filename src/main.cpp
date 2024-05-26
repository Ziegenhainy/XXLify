#include <Geode/modify/LevelInfoLayer.hpp>
#include "cvoltonLevelTime.cpp"
#include <cmath>
#include <thread>

using namespace geode::prelude;

std::string createXLString(int levelLengthMinutes) {
	std::string XLlabel;
	int maximumXs = Mod::get()->getSettingValue<int64_t>("maximum-xs");
	bool usingPowerNotation = Mod::get()->getSettingValue<bool>("use-power-notation");
	if (usingPowerNotation) {
		XLlabel.append("X^");
		XLlabel.append(std::to_string((int) log2(levelLengthMinutes)));
	}
	else {
		for (int i = 0; i < (int) log2(levelLengthMinutes) & i < maximumXs; i++) {
			XLlabel.append("X");
		}
	}
	XLlabel.append("L");
	return XLlabel;
};

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
	// Using CVolton's time calculator for levels before 2.2
	void createXLlabelCvolton() {
		this->retain();
		
		std::thread([this](){
			thread::setName("CVoltonTime");
			int cvoltonLengthMinutes = timeForLevelString(m_level->m_levelString);

			Loader::get()->queueInMainThread([this,cvoltonLengthMinutes]() {
				if (cvoltonLengthMinutes >= 4) {
					m_lengthLabel->setString(createXLString(cvoltonLengthMinutes).c_str());
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

		if (levelLengthMinutes < 4) {
			return;
		}

		m_lengthLabel->setString(createXLString(levelLengthMinutes).c_str());
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