#include <Geode/modify/LevelInfoLayer.hpp>
#include "cvoltonLevelTime.cpp"
#include <cmath>
#include <thread>

using namespace geode::prelude;

int logBaseN(float x, float n) {
	return std::log(x)/std::log(n);
}

std::string createXLstring(float levelLengthMinutes) {
	std::stringstream XLstring;
	int maximumXs           = Mod::get()->getSettingValue<int64_t>("maximum-xs");
	bool usingPowerNotation = Mod::get()->getSettingValue<bool>("use-power-notation");
	bool usingXXLplus       = Mod::get()->getSettingValue<bool>("xxl-plus");
	float xxlScaling        = Mod::get()->getSettingValue<double>("xxl-scaling");
	int lengthExponent      = logBaseN(levelLengthMinutes/2, xxlScaling)+1;

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

    float XXLPlusLength = pow(xxlScaling, lengthExponent) + pow(xxlScaling, lengthExponent - 1);
    if (levelLengthMinutes >= XXLPlusLength) {
		XLstring << "+";
    }

    return XLstring.str();
};

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
	void modifyXLlabel(int levelLengthMinutes) {
		bool usingCol = Mod::get()->getSettingValue<bool>("use-color");
		m_lengthLabel->setString(createXLstring(levelLengthMinutes).c_str());
		if (usingCol) {			
			int maxCol = Mod::get()->getSettingValue<int64_t>("maximum-color");
			ccColor3B labelCol = Mod::get()->getSettingValue<ccColor3B>("label-color");
			float colT=levelLengthMinutes<maxCol ? (float) levelLengthMinutes/maxCol : 1;

			labelCol.r = std::lerp(255, labelCol.r, colT);
			labelCol.g = std::lerp(255, labelCol.g, colT);
			labelCol.b = std::lerp(255, labelCol.b, colT);

			m_lengthLabel->setColor({labelCol.r, labelCol.g, labelCol.b});
		}
	}

	// Using CVolton's time calculator for levels before 2.2
	void createXLlabelCvolton() {
		this->retain();
		
		std::thread([this](){
			thread::setName("CVoltonTime");
			float cvoltonLengthMinutes = timeForLevelString(m_level->m_levelString);

			Loader::get()->queueInMainThread([this,cvoltonLengthMinutes]() {
				if (cvoltonLengthMinutes >= 2.0f) {
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
		
		float levelLengthMinutes = (float) m_level->m_timestamp / 14400.0f;
		
		
		if (levelLengthMinutes<=0.0f) {
			createXLlabelCvolton();
			return;
		}

		if (levelLengthMinutes < 2.0f) {
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