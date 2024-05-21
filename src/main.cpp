#include <Geode/modify/LevelInfoLayer.hpp>
#include <cmath>

using namespace geode::prelude;

std::string createXLString(int levelLengthMinutes) {
	std::string XLlabel;
	for (int i = 0; i < log2(levelLengthMinutes) & i < 10; i++) {
		XLlabel = XLlabel.append("X");
	}
	XLlabel = XLlabel.append("L");
	return XLlabel;
};

class $modify(MyLevelInfoLayer, LevelInfoLayer) {

	void createXLlabel() {
		if (m_level->isPlatformer()) {
			return;
		}
		
		int levelLengthMinutes = m_level->m_timestamp / 14400;
		
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