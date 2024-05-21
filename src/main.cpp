#include <Geode/modify/LevelInfoLayer.hpp>
#include <math.h>
#include <string>
#include <fmt/format.h>

using namespace geode::prelude;

std::string createXLlabel(int levelLengthMinutes) {
	std::string XLlabel;
	int numberOfXs = log2(levelLengthMinutes);
	log::info("Number of Xs is {}", numberOfXs);
	for (int i=0; i<numberOfXs & i<10; i++) {
		XLlabel = XLlabel.append("X");
	}
	XLlabel = XLlabel.append("L");
	return XLlabel;
};

class $modify(LevelInfoLayer) {

	bool XLlabelRendered;

	void displayXLlabel() {
		if (m_level->isPlatformer()) {
			return;
		}

		int levelLengthMinutes = m_level->m_timestamp/14400;
		log::info("length in minutes is {}", levelLengthMinutes);

		if (levelLengthMinutes < 4) {
			return;
		}

		std::string XLlabel = createXLlabel(levelLengthMinutes);
		m_lengthLabel->setString(fmt::format("{}", XLlabel).c_str());
	}

	void setupLevelInfo() {
		LevelInfoLayer::setupLevelInfo();
		displayXLlabel();
	}

	virtual void levelDownloadFinished(GJGameLevel* p0) {
		LevelInfoLayer::levelDownloadFinished(p0);
		displayXLlabel();
	}
};