#include "Etterna/Globals/global.h"
#include "CommonMetrics.h"
#include "Etterna/Singletons/GameState.h"
#include "Etterna/Singletons/NoteSkinManager.h"
#include "PlayerOptions.h"
#include "RageUtil/Utils/RageUtil.h"
#include "Etterna/Models/Songs/Song.h"
#include "Etterna/Models/StepsAndStyles/Steps.h"
#include "Etterna/Models/StepsAndStyles/Style.h"
#include "Etterna/Globals/rngthing.h"

#include <cmath>

static const char* LifeTypeNames[] = {
	"Bar",
	"Battery",
	"Time",
};
XToString(LifeType);
XToLocalizedString(LifeType);
LuaXType(LifeType);

static const char* DrainTypeNames[] = {
	"Normal",
	"NoRecover",
	"SuddenDeath",
};
XToString(DrainType);
XToLocalizedString(DrainType);
LuaXType(DrainType);

void
NextFloat(float fValues[], int size);
void
NextBool(bool bValues[], int size);

ThemeMetric<float> RANDOM_SPEED_CHANCE("PlayerOptions", "RandomSpeedChance");
ThemeMetric<float> RANDOM_REVERSE_CHANCE("PlayerOptions",
										 "RandomReverseChance");
ThemeMetric<float> RANDOM_DARK_CHANCE("PlayerOptions", "RandomDarkChance");
ThemeMetric<float> RANDOM_ACCEL_CHANCE("PlayerOptions", "RandomAccelChance");
ThemeMetric<float> RANDOM_EFFECT_CHANCE("PlayerOptions", "RandomEffectChance");
ThemeMetric<float> RANDOM_HIDDEN_CHANCE("PlayerOptions", "RandomHiddenChance");
ThemeMetric<float> RANDOM_SUDDEN_CHANCE("PlayerOptions", "RandomSuddenChance");

static const float CMOD_DEFAULT = 200.0f;
// Is there a better place for this?
// It needs to be a named constant because it's used in several places in
// this file, but nothing else has a named constant for its default value.
// -Kyz

void
PlayerOptions::Init()
{
	m_LifeType = LifeType_Bar;
	m_DrainType = DrainType_Normal;
	m_BatteryLives = 4;
	m_MinTNSToHideNotes = PREFSMAN->m_MinTNSToHideNotes;
	m_bSetScrollSpeed = false;
	m_fMaxScrollBPM = 0;
	m_SpeedfMaxScrollBPM = 1.0f;
	m_fTimeSpacing = 0;
	m_SpeedfTimeSpacing = 1.0f;
	m_fScrollSpeed = 1.0f;
	m_SpeedfScrollSpeed = 1.0f;
	m_fScrollBPM = CMOD_DEFAULT;
	m_SpeedfScrollBPM = 1.0f;
	ZERO(m_fAccels);
	ONE(m_SpeedfAccels);
	ZERO(m_fEffects);
	ONE(m_SpeedfEffects);
	ZERO(m_fAppearances);
	ONE(m_SpeedfAppearances);
	ZERO(m_fScrolls);
	ONE(m_SpeedfScrolls);
	ZERO(m_bTurns);
	ZERO(m_bTransforms);
	m_fDark = 0;
	m_SpeedfDark = 1.0f;
	m_fBlind = 0;
	m_SpeedfBlind = 1.0f;
	m_fCover = 0;
	m_SpeedfCover = 1.0f;
	m_fRandAttack = 0;
	m_SpeedfRandAttack = 1.0f;
	m_fNoAttack = 0;
	m_SpeedfNoAttack = 1.0f;
	m_fPlayerAutoPlay = 0;
	m_SpeedfPlayerAutoPlay = 1.0f;
	m_fPerspectiveTilt = 0;
	m_SpeedfPerspectiveTilt = 1.0f;
	m_fSkew = 0;
	m_SpeedfSkew = 1.0f;
	m_fPassmark = 0;
	m_SpeedfPassmark = 1.0f;
	m_fRandomSpeed = 0;
	m_SpeedfRandomSpeed = 1.0f;
	m_bMuteOnError = false;
	m_bPractice = false;
	m_sNoteSkin = "";
	m_bStealthType = false;
	m_bStealthPastReceptors = false;
	m_bCosecant = false;
	ZERO(m_fMovesX);
	ONE(m_SpeedfMovesX);
	ZERO(m_fMovesY);
	ONE(m_SpeedfMovesY);
	ZERO(m_fMovesZ);
	ONE(m_SpeedfMovesZ);
	ZERO(m_fBumpy);
	ONE(m_SpeedfBumpy);
	ZERO(m_fTiny);
	ONE(m_SpeedfTiny);
	ZERO(m_fConfusionZ);
	ONE(m_SpeedfConfusionZ);
	ZERO(m_fConfusionY);
	ONE(m_SpeedfConfusionY);
	ZERO(m_fConfusionZ);
	ONE(m_SpeedfConfusionZ);
	ZERO(m_fStealth);
	ONE(m_SpeedfStealth);
	ZERO(m_fReverse);
	ONE(m_SpeedfReverse);
}

void
PlayerOptions::Approach(const PlayerOptions& other, float fDeltaSeconds)
{
#define APPROACH(opt)                                                          \
	fapproach(m_##opt, other.m_##opt, fDeltaSeconds* other.m_Speed##opt);
#define DO_COPY(x) x = other.x;

	DO_COPY(m_LifeType);
	DO_COPY(m_DrainType);
	DO_COPY(m_BatteryLives);
	APPROACH(fTimeSpacing);
	APPROACH(fScrollSpeed);
	APPROACH(fMaxScrollBPM);
	fapproach(m_fScrollBPM,
			  other.m_fScrollBPM,
			  fDeltaSeconds * other.m_SpeedfScrollBPM * 150);
	for (auto i = 0; i < NUM_ACCELS; i++)
		APPROACH(fAccels[i]);
	for (auto i = 0; i < NUM_EFFECTS; i++)
		APPROACH(fEffects[i]);
	for (auto i = 0; i < NUM_APPEARANCES; i++)
		APPROACH(fAppearances[i]);
	for (auto i = 0; i < NUM_SCROLLS; i++)
		APPROACH(fScrolls[i]);
	APPROACH(fDark);
	APPROACH(fBlind);
	APPROACH(fCover);
	APPROACH(fRandAttack);
	APPROACH(fNoAttack);
	APPROACH(fPlayerAutoPlay);
	APPROACH(fPerspectiveTilt);
	APPROACH(fSkew);
	APPROACH(fPassmark);
	APPROACH(fRandomSpeed);
	DO_COPY(m_bSetScrollSpeed);
	for (auto i = 0; i < NUM_TURNS; i++)
		DO_COPY(m_bTurns[i]);
	for (auto i = 0; i < NUM_TRANSFORMS; i++)
		DO_COPY(m_bTransforms[i]);
	DO_COPY(m_bMuteOnError);
	DO_COPY(m_FailType);
	DO_COPY(m_MinTNSToHideNotes);
	DO_COPY(m_sNoteSkin);
	DO_COPY(m_bCosecant);
	for (int i = 0; i < 16; i++)
		APPROACH(fMovesX[i]);
	for (int i = 0; i < 16; i++)
		APPROACH(fMovesY[i]);
	for (int i = 0; i < 16; i++)
		APPROACH(fMovesZ[i]);
	for (int i = 0; i < 16; i++)
		APPROACH(fBumpy[i]);
	for (int i = 0; i < 16; i++)
		APPROACH(fTiny[i]);
	for (int i = 0; i < 16; i++)
		APPROACH(fConfusionX[i]);
	for (int i = 0; i < 16; i++)
		APPROACH(fConfusionY[i]);
	for (int i = 0; i < 16; i++)
		APPROACH(fConfusionZ[i]);
	for (int i = 0; i < 16; i++)
		APPROACH(fStealth[i]);
	for (int i = 0; i < 16; i++)
		APPROACH(fReverse[i]);
	DO_COPY(m_bStealthType);
	DO_COPY(m_bStealthPastReceptors);
#undef APPROACH
#undef DO_COPY
}

static void
AddPart(vector<std::string>& AddTo, float level, std::string name)
{
	if (level == 0)
		return;

	const auto LevelStr =
	  (level == 1) ? std::string("") : ssprintf("%ld%% ", lround(level * 100));

	AddTo.push_back(LevelStr + name);
}

std::string
PlayerOptions::GetString(bool bForceNoteSkin) const
{
	vector<std::string> v;
	GetMods(v, bForceNoteSkin);
	return join(", ", v);
}

void
PlayerOptions::GetMods(vector<std::string>& AddTo, bool bForceNoteSkin) const
{
	// std::string sReturn;

	switch (m_LifeType) {
		case LifeType_Bar:
			switch (m_DrainType) {
				case DrainType_NoRecover:
					AddTo.push_back("NoRecover");
					break;
				case DrainType_SuddenDeath:
					AddTo.push_back("SuddenDeath");
					break;
				case DrainType_Normal:
				default:
					break;
			}
			break;
		case LifeType_Battery:
			AddTo.push_back(ssprintf("%dLives", m_BatteryLives));
			break;
		case LifeType_Time:
			AddTo.push_back("LifeTime");
			break;
		default:
			FAIL_M(ssprintf("Invalid LifeType: %i", m_LifeType));
	}

	if (!m_fTimeSpacing) {
		if (m_fMaxScrollBPM) {
			const auto s = ssprintf("m%.0f", m_fMaxScrollBPM);
			AddTo.push_back(s);
		} else if (m_bSetScrollSpeed || m_fScrollSpeed != 1) {
			/* -> 1.00 */
			auto s = ssprintf("%2.2f", m_fScrollSpeed);
			if (s[s.size() - 1] == '0') {
				/* -> 1.0 */
				s.erase(s.size() - 1); // delete last char
				if (s[s.size() - 1] == '0') {
					/* -> 1 */
					s.erase(s.size() - 2); // delete last 2 chars
				}
			}
			AddTo.push_back(s + "x");
		}
	} else {
		const auto s = ssprintf("C%.0f", m_fScrollBPM);
		AddTo.push_back(s);
	}

	AddPart(AddTo, m_fAccels[ACCEL_BOOST], "Boost");
	AddPart(AddTo, m_fAccels[ACCEL_BRAKE], "Brake");
	AddPart(AddTo, m_fAccels[ACCEL_WAVE], "Wave");
	AddPart(AddTo, m_fAccels[ACCEL_WAVE_PERIOD], "WavePeriod");
	AddPart(AddTo, m_fAccels[ACCEL_EXPAND], "Expand");
	AddPart(AddTo, m_fAccels[ACCEL_EXPAND_PERIOD], "ExpandPeriod");
	AddPart(AddTo, m_fAccels[ACCEL_TAN_EXPAND], "TanExpand");
	AddPart(AddTo, m_fAccels[ACCEL_TAN_EXPAND_PERIOD], "TanExpandPeriod");
	AddPart(AddTo, m_fAccels[ACCEL_BOOMERANG], "Boomerang");

	AddPart(AddTo, m_fEffects[EFFECT_DRUNK], "Drunk");
	AddPart(AddTo, m_fEffects[EFFECT_DIZZY], "Dizzy");
	AddPart(AddTo, m_fEffects[EFFECT_CONFUSION], "Confusion");
	AddPart(AddTo, m_fEffects[EFFECT_CONFUSION_OFFSET], "ConfusionOffset");
	AddPart(AddTo, m_fEffects[EFFECT_CONFUSION_X], "ConfusionX");
	AddPart(AddTo, m_fEffects[EFFECT_CONFUSION_X_OFFSET], "ConfusionXOffset");
	AddPart(AddTo, m_fEffects[EFFECT_CONFUSION_Y], "ConfusionY");
	AddPart(AddTo, m_fEffects[EFFECT_CONFUSION_Y_OFFSET], "ConfusionYOffset");
	AddPart(AddTo, m_fEffects[EFFECT_MINI], "Mini");
	AddPart(AddTo, m_fEffects[EFFECT_TINY], "Tiny");
	AddPart(AddTo, m_fEffects[EFFECT_FLIP], "Flip");
	AddPart(AddTo, m_fEffects[EFFECT_INVERT], "Invert");
	AddPart(AddTo, m_fEffects[EFFECT_TORNADO], "Tornado");
	AddPart(AddTo, m_fEffects[EFFECT_TIPSY], "Tipsy");
	AddPart(AddTo, m_fEffects[EFFECT_BUMPY], "Bumpy");
	AddPart(AddTo, m_fEffects[EFFECT_BEAT], "Beat");
	AddPart(AddTo, m_fEffects[EFFECT_XMODE], "XMode");
	AddPart(AddTo, m_fEffects[EFFECT_TWIRL], "Twirl");
	AddPart(AddTo, m_fEffects[EFFECT_ROLL], "Roll");
	AddPart(AddTo, m_fEffects[EFFECT_ATTENUATE_X], "AttenuateX");
	AddPart(AddTo, m_fEffects[EFFECT_ATTENUATE_Y], "AttenuateY");
	AddPart(AddTo, m_fEffects[EFFECT_ATTENUATE_Z], "AttenuateZ");
	AddPart(AddTo, m_fEffects[EFFECT_BOUNCE], "Bounce");
	AddPart(AddTo, m_fEffects[EFFECT_BOUNCE_PERIOD], "BouncePeriod");
	AddPart(AddTo, m_fEffects[EFFECT_BOUNCE_OFFSET], "BounceOffset");
	AddPart(AddTo, m_fEffects[EFFECT_BOUNCE_Z], "BounceZ");
	AddPart(AddTo, m_fEffects[EFFECT_BOUNCE_Z_PERIOD], "BounceZPeriod");
	AddPart(AddTo, m_fEffects[EFFECT_BOUNCE_Z_OFFSET], "BounceZOffset");
	AddPart(AddTo, m_fEffects[EFFECT_DIGITAL], "Digital");
	AddPart(AddTo, m_fEffects[EFFECT_DIGITAL_STEPS], "DigitalSteps");
	AddPart(AddTo, m_fEffects[EFFECT_DIGITAL_PERIOD], "DigitalPeriod");
	AddPart(AddTo, m_fEffects[EFFECT_DIGITAL_OFFSET], "DigitalOffset");
	AddPart(AddTo, m_fEffects[EFFECT_TAN_DIGITAL], "TanDigital");
	AddPart(AddTo, m_fEffects[EFFECT_TAN_DIGITAL_STEPS], "TanDigitalSteps");
	AddPart(AddTo, m_fEffects[EFFECT_TAN_DIGITAL_PERIOD], "TanDigitalPeriod");
	AddPart(AddTo, m_fEffects[EFFECT_TAN_DIGITAL_OFFSET], "TanDigitalOffset");
	AddPart(AddTo, m_fEffects[EFFECT_DIGITAL_Z], "DigitalZ");
	AddPart(AddTo, m_fEffects[EFFECT_DIGITAL_Z_STEPS], "DigitalZSteps");
	AddPart(AddTo, m_fEffects[EFFECT_DIGITAL_Z_PERIOD], "DigitalZPeriod");
	AddPart(AddTo, m_fEffects[EFFECT_DIGITAL_Z_OFFSET], "DigitalZOffset");
	AddPart(AddTo, m_fEffects[EFFECT_TAN_DIGITAL_Z], "TanDigitalZ");
	AddPart(AddTo, m_fEffects[EFFECT_TAN_DIGITAL_Z_STEPS], "TanDigitalZSteps");
	AddPart(
	  AddTo, m_fEffects[EFFECT_TAN_DIGITAL_Z_PERIOD], "TanDigitalZPeriod");
	AddPart(
	  AddTo, m_fEffects[EFFECT_TAN_DIGITAL_Z_OFFSET], "TanDigitalZOffset");
	AddPart(AddTo, m_fEffects[EFFECT_ZIGZAG], "Zigzag");
	AddPart(AddTo, m_fEffects[EFFECT_ZIGZAG_PERIOD], "ZigzagPeriod");
	AddPart(AddTo, m_fEffects[EFFECT_ZIGZAG_OFFSET], "ZigzagOffset");
	AddPart(AddTo, m_fEffects[EFFECT_ZIGZAG_Z], "ZigzagZ");
	AddPart(AddTo, m_fEffects[EFFECT_ZIGZAG_Z_PERIOD], "ZigzagZPeriod");
	AddPart(AddTo, m_fEffects[EFFECT_ZIGZAG_Z_OFFSET], "ZigzagZOffset");

	AddPart(AddTo, m_fAppearances[APPEARANCE_HIDDEN], "Hidden");
	AddPart(AddTo, m_fAppearances[APPEARANCE_HIDDEN_OFFSET], "HiddenOffset");
	AddPart(AddTo, m_fAppearances[APPEARANCE_SUDDEN], "Sudden");
	AddPart(AddTo, m_fAppearances[APPEARANCE_SUDDEN_OFFSET], "SuddenOffset");
	AddPart(AddTo, m_fAppearances[APPEARANCE_STEALTH], "Stealth");
	AddPart(AddTo, m_fAppearances[APPEARANCE_BLINK], "Blink");
	AddPart(AddTo, m_fAppearances[APPEARANCE_RANDOMVANISH], "RandomVanish");

	AddPart(AddTo, m_fScrolls[SCROLL_REVERSE], "Reverse");
	AddPart(AddTo, m_fScrolls[SCROLL_SPLIT], "Split");
	AddPart(AddTo, m_fScrolls[SCROLL_ALTERNATE], "Alternate");
	AddPart(AddTo, m_fScrolls[SCROLL_CROSS], "Cross");
	AddPart(AddTo, m_fScrolls[SCROLL_CENTERED], "Centered");

	AddPart(AddTo, m_fDark, "Dark");
	AddPart(AddTo, m_fBlind, "Blind");
	AddPart(AddTo, m_fCover, "Cover");

	AddPart(AddTo, m_fRandAttack, "RandomAttacks");
	AddPart(AddTo, m_fNoAttack, "NoAttacks");
	AddPart(AddTo, m_fPlayerAutoPlay, "PlayerAutoPlay");
	AddPart(AddTo, m_fPassmark, "Passmark");
	AddPart(AddTo, m_fRandomSpeed, "RandomSpeed");
	AddPart(AddTo, m_bCosecant, "Cosecant");
	AddPart(AddTo, m_bStealthType, "StealthType");
	AddPart(AddTo, m_bStealthPastReceptors, "StealthPastReceptors");

	for (int i = 0; i < 16; i++) {
		auto s = ssprintf("MoveX%d", i + 1);
		AddPart(AddTo, m_fMovesX[i], s);
		s = ssprintf("MoveY%d", i + 1);
		AddPart(AddTo, m_fMovesY[i], s);
		s = ssprintf("MoveZ%d", i + 1);
		AddPart(AddTo, m_fMovesZ[i], s);
		s = ssprintf("Stealth%d", i + 1);
		AddPart(AddTo, m_fStealth[i], s);
		s = ssprintf("Bumpy%d", i + 1);
		AddPart(AddTo, m_fBumpy[i], s);
		s = ssprintf("Tiny%d", i + 1);
		AddPart(AddTo, m_fTiny[i], s);
		s = ssprintf("ConfusionXOffset%d", i + 1);
		AddPart(AddTo, m_fConfusionX[i], s);
		s = ssprintf("ConfusionYOffset%d", i + 1);
		AddPart(AddTo, m_fConfusionY[i], s);
		s = ssprintf("ConfusionZOffset%d", i + 1);
		AddPart(AddTo, m_fConfusionZ[i], s);
		s = ssprintf("Reverse%d", i + 1);
		AddPart(AddTo, m_fReverse[i], s);
	}

	if (m_bTurns[TURN_MIRROR])
		AddTo.push_back("Mirror");
	if (m_bTurns[TURN_BACKWARDS])
		AddTo.push_back("Backwards");
	if (m_bTurns[TURN_LEFT])
		AddTo.push_back("Left");
	if (m_bTurns[TURN_RIGHT])
		AddTo.push_back("Right");
	if (m_bTurns[TURN_SHUFFLE])
		AddTo.push_back("Shuffle");
	if (m_bTurns[TURN_SOFT_SHUFFLE])
		AddTo.push_back("SoftShuffle");
	if (m_bTurns[TURN_SUPER_SHUFFLE])
		AddTo.push_back("SuperShuffle");

	if (m_bTransforms[TRANSFORM_NOHOLDS])
		AddTo.push_back("NoHolds");
	if (m_bTransforms[TRANSFORM_NOROLLS])
		AddTo.push_back("NoRolls");
	if (m_bTransforms[TRANSFORM_NOMINES])
		AddTo.push_back("NoMines");
	if (m_bTransforms[TRANSFORM_LITTLE])
		AddTo.push_back("Little");
	if (m_bTransforms[TRANSFORM_WIDE])
		AddTo.push_back("Wide");
	if (m_bTransforms[TRANSFORM_BIG])
		AddTo.push_back("Big");
	if (m_bTransforms[TRANSFORM_QUICK])
		AddTo.push_back("Quick");
	if (m_bTransforms[TRANSFORM_BMRIZE])
		AddTo.push_back("BMRize");
	if (m_bTransforms[TRANSFORM_SKIPPY])
		AddTo.push_back("Skippy");
	if (m_bTransforms[TRANSFORM_MINES])
		AddTo.push_back("Mines");
	if (m_bTransforms[TRANSFORM_ATTACKMINES])
		AddTo.push_back("AttackMines");
	if (m_bTransforms[TRANSFORM_ECHO])
		AddTo.push_back("Echo");
	if (m_bTransforms[TRANSFORM_STOMP])
		AddTo.push_back("Stomp");
	if (m_bTransforms[TRANSFORM_JACKJS])
		AddTo.push_back("JackJS");
	if (m_bTransforms[TRANSFORM_ANCHORJS])
		AddTo.push_back("AnchorJS");
	if (m_bTransforms[TRANSFORM_ICYWORLD])
		AddTo.push_back("IcyWorld");
	if (m_bTransforms[TRANSFORM_PLANTED])
		AddTo.push_back("Planted");
	if (m_bTransforms[TRANSFORM_FLOORED])
		AddTo.push_back("Floored");
	if (m_bTransforms[TRANSFORM_TWISTER])
		AddTo.push_back("Twister");
	if (m_bTransforms[TRANSFORM_HOLDROLLS])
		AddTo.push_back("HoldsToRolls");
	if (m_bTransforms[TRANSFORM_NOJUMPS])
		AddTo.push_back("NoJumps");
	if (m_bTransforms[TRANSFORM_NOHANDS])
		AddTo.push_back("NoHands");
	if (m_bTransforms[TRANSFORM_NOLIFTS])
		AddTo.push_back("NoLifts");
	if (m_bTransforms[TRANSFORM_NOFAKES])
		AddTo.push_back("NoFakes");
	if (m_bTransforms[TRANSFORM_NOQUADS])
		AddTo.push_back("NoQuads");
	if (m_bTransforms[TRANSFORM_NOSTRETCH])
		AddTo.push_back("NoStretch");
	if (m_bMuteOnError)
		AddTo.push_back("MuteOnError");
	if (m_bPractice)
		AddTo.push_back("PracticeMode");

	switch (m_FailType) {
		case FailType_Immediate:
			break;
		case FailType_ImmediateContinue:
			AddTo.push_back("FailImmediateContinue");
			break;
		case FailType_Off:
			AddTo.push_back("FailOff");
			break;
		default:
			FAIL_M(ssprintf("Invalid FailType: %i", m_FailType));
	}

	if (m_fSkew == 0 && m_fPerspectiveTilt == 0) {
		AddTo.push_back("Overhead");
	} else if (m_fSkew == 0) {
		if (m_fPerspectiveTilt > 0)
			AddPart(AddTo, m_fPerspectiveTilt, "Distant");
		else
			AddPart(AddTo, -m_fPerspectiveTilt, "Hallway");
	} else if (fabsf(m_fSkew - m_fPerspectiveTilt) < 0.0001f) {
		AddPart(AddTo, m_fSkew, "Space");
	} else if (fabsf(m_fSkew + m_fPerspectiveTilt) < 0.0001f) {
		AddPart(AddTo, m_fSkew, "Incoming");
	} else {
		AddPart(AddTo, m_fSkew, "Skew");
		AddPart(AddTo, m_fPerspectiveTilt, "Tilt");
	}

	// Don't display a string if using the default NoteSkin unless we force it.
	if (bForceNoteSkin ||
		(!m_sNoteSkin.empty() &&
		 m_sNoteSkin != CommonMetrics::DEFAULT_NOTESKIN_NAME.GetValue())) {
		const auto s = m_sNoteSkin;
		Capitalize(s);
		AddTo.push_back(s);
	}
}

void
PlayerOptions::GetTurnMods(vector<std::string>& AddTo)
{
	if (m_bTurns[TURN_MIRROR])
		AddTo.push_back("Mirror");
	if (m_bTurns[TURN_BACKWARDS])
		AddTo.push_back("Backwards");
	if (m_bTurns[TURN_LEFT])
		AddTo.push_back("Left");
	if (m_bTurns[TURN_RIGHT])
		AddTo.push_back("Right");
	if (m_bTurns[TURN_SHUFFLE])
		AddTo.push_back("Shuffle");
	if (m_bTurns[TURN_SOFT_SHUFFLE])
		AddTo.push_back("SoftShuffle");
	if (m_bTurns[TURN_SUPER_SHUFFLE])
		AddTo.push_back("SuperShuffle");
}

// At the moment this only supports turns.
// Do not use this for anything else.
// It will not work.
void
PlayerOptions::ResetModsToStringVector(vector<std::string> mods)
{
	std::string error;
	ResetToggleableMods();
	for (const auto& mod : mods) {
		FromOneModString(mod, error);
	}
}

// At the moment this only supports turns.
// Do not use this for anything else.
// It will not work.
void
PlayerOptions::ResetToggleableMods()
{
	ZERO(m_bTurns);
}

/* Options are added to the current settings; call Init() beforehand if
 * you don't want this. */
void
PlayerOptions::FromString(const std::string& sMultipleMods)
{
	const auto sTemp = sMultipleMods;
	vector<std::string> vs;
	split(sTemp, ",", vs, true);
	std::string sThrowAway;
	for (auto& s : vs) {
		if (!FromOneModString(s, sThrowAway)) {
			LOG->Trace("Attempted to load a non-existing mod \'%s\' for the "
					   "Player. Ignoring.",
					   (s).c_str());
		}
	}
}

bool
PlayerOptions::FromOneModString(const std::string& sOneMod,
								std::string& sErrorOut)
{
	ASSERT_M(NOTESKIN != NULL,
			 "The Noteskin Manager must be loaded in order to process mods.");

	auto sBit = make_lower(sOneMod);
	std::string sMod = "";
	Trim(sBit);

	/* "drunk"
	 * "no drunk"
	 * "150% drunk"
	 * "*2 100% drunk": approach at 2x normal speed */

	float level = 1;
	float speed = 1;
	vector<std::string> asParts;
	split(sBit, " ", asParts, true);

	for (auto& s : asParts) {
		if (s == "no") {
			level = 0;
		} else if (isdigit(s[0]) || s[0] == '-') {
			/* If the last character is a *, they probably said "123*" when
			 * they meant "*123". */
			if (s.back() == '*') {
				// XXX: We know what they want, is there any reason not to
				// handle it? Yes. We should be strict in handling the format.
				// -Chris
				sErrorOut =
				  ssprintf("Invalid player options \"%s\"; did you mean '*%d'?",
						   s.c_str(),
						   StringToInt(s));
				return false;
			} else {
				level = StringToFloat(s) / 100.0f;
			}
		} else if (s[0] == '*') {
			sscanf(s.c_str(), "*%f", &speed);
			if (!std::isfinite(speed))
				speed = 1.0f;
		}
	}

	sBit = asParts.back();

#define SET_FLOAT(opt)                                                         \
	{                                                                          \
		m_##opt = level;                                                       \
		m_Speed##opt = speed;                                                  \
	}
	const auto on = (level > 0.5f);

	static Regex mult("^([0-9]+(\\.[0-9]+)?)x$");
	vector<std::string> matches;
	if (mult.Compare(sBit, matches)) {
		StringConversion::FromString(matches[0], level);
		SET_FLOAT(fScrollSpeed)
		SET_FLOAT(fTimeSpacing)
		m_fTimeSpacing = 0;
		m_fMaxScrollBPM = 0;
	} else if (sscanf(sBit.c_str(), "c%f", &level) == 1) {
		if (!std::isfinite(level) || level <= 0.0f)
			level = CMOD_DEFAULT;
		SET_FLOAT(fScrollBPM)
		SET_FLOAT(fTimeSpacing)
		m_fTimeSpacing = 1;
		m_fMaxScrollBPM = 0;
	}
	// oITG's m-mods
	else if (sscanf(sBit.c_str(), "m%f", &level) == 1) {
		// OpenITG doesn't have this block:
		/*
		if( !isfinite(level) || level <= 0.0f )
			level = CMOD_DEFAULT;
		*/
		SET_FLOAT(fMaxScrollBPM)
		m_fTimeSpacing = 0;
	}

	else if (sBit == "clearall") {
		Init();
		m_sNoteSkin = NOTESKIN->GetDefaultNoteSkinName();
	} else if (sBit == "resetspeed") {
		/* level is set to the values from Init() because all speed related
		   fields are being reset to initial values, and they each have
		   different initial values.  -kyz */
		level = 0;
		SET_FLOAT(fMaxScrollBPM);
		SET_FLOAT(fTimeSpacing);
		level = 1.0f;
		SET_FLOAT(fScrollSpeed);
		level = CMOD_DEFAULT;
		SET_FLOAT(fScrollBPM)
	} else if (sBit == "life" || sBit == "lives") {
		// level is a percentage for every other option, so multiply by 100.
		// -Kyz
		m_BatteryLives = static_cast<int>(level * 100.0f);
	} else if (sBit == "bar") {
		m_LifeType = LifeType_Bar;
	} else if (sBit == "battery") {
		m_LifeType = LifeType_Battery;
	} else if (sBit == "lifetime") {
		m_LifeType = LifeType_Time;
	} else if (sBit == "norecover" || sBit == "power-drop") {
		m_DrainType = DrainType_NoRecover;
	} else if (sBit == "suddendeath" || sBit == "death") {
		m_DrainType = DrainType_SuddenDeath;
	} else if (sBit == "normal-drain") {
		m_DrainType = DrainType_Normal;
	} else if (sBit == "boost")
		SET_FLOAT(fAccels[ACCEL_BOOST])
	else if (sBit == "brake" || sBit == "land")
		SET_FLOAT(fAccels[ACCEL_BRAKE])
	else if (sBit.find("wave") != sBit.npos) {
		if (sBit == "wave")
			SET_FLOAT(fAccels[ACCEL_WAVE])
		else if (sBit == "waveperiod")
			SET_FLOAT(fAccels[ACCEL_WAVE_PERIOD])
	} else if ((sBit.find("expand") != sBit.npos) ||
			   (sBit.find("dwiwave") != sBit.npos)) {
		if (sBit == "expand" || sBit == "dwiwave")
			SET_FLOAT(fAccels[ACCEL_EXPAND])
		else if (sBit == "expandperiod")
			SET_FLOAT(fAccels[ACCEL_EXPAND_PERIOD])
		else if (sBit == "tanexpand")
			SET_FLOAT(fAccels[ACCEL_TAN_EXPAND])
		else if (sBit == "tanexpandperiod")
			SET_FLOAT(fAccels[ACCEL_TAN_EXPAND_PERIOD])
	} else if (sBit == "boomerang")
		SET_FLOAT(fAccels[ACCEL_BOOMERANG])
	else if (sBit == "drunk")
		SET_FLOAT(fEffects[EFFECT_DRUNK])
	else if (sBit == "dizzy")
		SET_FLOAT(fEffects[EFFECT_DIZZY])
	else if (sBit.find("confusion") != sBit.npos) {
		if (sBit.find("x") != sBit.npos) {
			if (sBit == "confusionx")
				SET_FLOAT(fEffects[EFFECT_CONFUSION_X])
			else if (sBit == "confusionxoffset")
				SET_FLOAT(fEffects[EFFECT_CONFUSION_X_OFFSET])
			else {
				for (int i = 0; i < 16; i++) {
					sMod = ssprintf("confusionxoffset%d", i + 1);
					if (sBit == sMod) {
						SET_FLOAT(fConfusionX[i])
						break;
					}
				}
			}
		} else if (sBit.find("y") != sBit.npos) {
			if (sBit == "confusiony")
				SET_FLOAT(fEffects[EFFECT_CONFUSION_Y])
			else if (sBit == "confusionyoffset")
				SET_FLOAT(fEffects[EFFECT_CONFUSION_Y_OFFSET])
			else {
				for (int i = 0; i < 16; i++) {
					sMod = ssprintf("confusionyoffset%d", i + 1);
					if (sBit == sMod) {
						SET_FLOAT(fConfusionY[i])
						break;
					}
				}
			}
		} else if (sBit == "confusion")
			SET_FLOAT(fEffects[EFFECT_CONFUSION])
		else if (sBit == "confusionoffset")
			SET_FLOAT(fEffects[EFFECT_CONFUSION_OFFSET])
		else {
			for (int i = 0; i < 16; i++) {
				sMod = ssprintf("confusionoffset%d", i + 1);
				if (sBit == sMod) {
					SET_FLOAT(fConfusionZ[i])
					break;
				}
			}
		}
	} else if (sBit == "mini")
		SET_FLOAT(fEffects[EFFECT_MINI])
	else if (sBit.find("tiny") != sBit.npos) {
		if (sBit == "tiny")
			SET_FLOAT(fEffects[EFFECT_TINY])
		else {
			for (int i = 0; i < 16; i++) {
				sMod = ssprintf("tiny%d", i + 1);
				if (sBit == sMod) {
					SET_FLOAT(fTiny[i])
					break;
				}
			}
		}
	} else if (sBit == "flip")
		SET_FLOAT(fEffects[EFFECT_FLIP])
	else if (sBit == "invert")
		SET_FLOAT(fEffects[EFFECT_INVERT])
	else if (sBit == "tornado")
		SET_FLOAT(fEffects[EFFECT_TORNADO])
	else if (sBit == "tipsy")
		SET_FLOAT(fEffects[EFFECT_TIPSY])
	else if (sBit.find("bumpy") != sBit.npos) {
		if (sBit == "bumpy")
			SET_FLOAT(fEffects[EFFECT_BUMPY])
		else {
			for (int i = 0; i < 16; i++) {
				sMod = ssprintf("bumpy%d", i + 1);
				if (sBit == sMod) {
					SET_FLOAT(fBumpy[i])
					break;
				}
			}
		}
	} else if (sBit == "beat")
		SET_FLOAT(fEffects[EFFECT_BEAT])
	else if (sBit == "xmode")
		SET_FLOAT(fEffects[EFFECT_XMODE])
	else if (sBit == "twirl")
		SET_FLOAT(fEffects[EFFECT_TWIRL])
	else if (sBit == "roll")
		SET_FLOAT(fEffects[EFFECT_ROLL])
	else if (sBit == "hidden")
		SET_FLOAT(fAppearances[APPEARANCE_HIDDEN])
	else if (sBit == "hiddenoffset")
		SET_FLOAT(fAppearances[APPEARANCE_HIDDEN_OFFSET])
	else if (sBit == "sudden")
		SET_FLOAT(fAppearances[APPEARANCE_SUDDEN])
	else if (sBit == "suddenoffset")
		SET_FLOAT(fAppearances[APPEARANCE_SUDDEN_OFFSET])
	else if (sBit.find("stealth") != sBit.npos) {
		if (sBit == "stealth")
			SET_FLOAT(fAppearances[APPEARANCE_STEALTH])
		else if (sBit == "stealthtype")
			m_bStealthType = on;
		else if (sBit == "stealthpastreceptors")
			m_bStealthPastReceptors = on;
		else {
			for (int i = 0; i < 16; i++) {
				sMod = ssprintf("stealth%d", i + 1);
				if (sBit == sMod) {
					SET_FLOAT(fStealth[i])
					break;
				}
			}
		}
	} else if (sBit.find("attenuate") != sBit.npos) {
		if (sBit == "attenuatex")
			SET_FLOAT(fEffects[EFFECT_ATTENUATE_X])
		else if (sBit == "attenuatey")
			SET_FLOAT(fEffects[EFFECT_ATTENUATE_Y])
		else if (sBit == "attenuatez")
			SET_FLOAT(fEffects[EFFECT_ATTENUATE_Z])
	} else if (sBit.find("bounce") != sBit.npos) {
		if (sBit == "bounce")
			SET_FLOAT(fEffects[EFFECT_BOUNCE])
		else if (sBit == "bounceperiod")
			SET_FLOAT(fEffects[EFFECT_BOUNCE_PERIOD])
		else if (sBit == "bounceoffset")
			SET_FLOAT(fEffects[EFFECT_BOUNCE_OFFSET])
		else if (sBit == "bouncez")
			SET_FLOAT(fEffects[EFFECT_BOUNCE_Z])
		else if (sBit == "bouncezperiod")
			SET_FLOAT(fEffects[EFFECT_BOUNCE_Z_PERIOD])
		else if (sBit == "bouncezoffset")
			SET_FLOAT(fEffects[EFFECT_BOUNCE_Z_OFFSET])
	} else if (sBit.find("digital") != sBit.npos) {
		if (sBit == "digital")
			SET_FLOAT(fEffects[EFFECT_DIGITAL])
		else if (sBit == "digitalsteps")
			SET_FLOAT(fEffects[EFFECT_DIGITAL_STEPS])
		else if (sBit == "digitalperiod")
			SET_FLOAT(fEffects[EFFECT_DIGITAL_PERIOD])
		else if (sBit == "digitaloffset")
			SET_FLOAT(fEffects[EFFECT_DIGITAL_OFFSET])
		else if (sBit == "tandigital")
			SET_FLOAT(fEffects[EFFECT_TAN_DIGITAL])
		else if (sBit == "tandigitalsteps")
			SET_FLOAT(fEffects[EFFECT_TAN_DIGITAL_STEPS])
		else if (sBit == "tandigitalperiod")
			SET_FLOAT(fEffects[EFFECT_TAN_DIGITAL_PERIOD])
		else if (sBit == "tandigitaloffset")
			SET_FLOAT(fEffects[EFFECT_TAN_DIGITAL_OFFSET])
		else if (sBit == "digitalz")
			SET_FLOAT(fEffects[EFFECT_DIGITAL_Z])
		else if (sBit == "digitalzsteps")
			SET_FLOAT(fEffects[EFFECT_DIGITAL_Z_STEPS])
		else if (sBit == "digitalzperiod")
			SET_FLOAT(fEffects[EFFECT_DIGITAL_Z_PERIOD])
		else if (sBit == "digitalzoffset")
			SET_FLOAT(fEffects[EFFECT_DIGITAL_Z_OFFSET])
		else if (sBit == "tandigitalz")
			SET_FLOAT(fEffects[EFFECT_TAN_DIGITAL_Z])
		else if (sBit == "tandigitalzsteps")
			SET_FLOAT(fEffects[EFFECT_TAN_DIGITAL_Z_STEPS])
		else if (sBit == "tandigitalzperiod")
			SET_FLOAT(fEffects[EFFECT_TAN_DIGITAL_Z_PERIOD])
		else if (sBit == "tandigitalzoffset")
			SET_FLOAT(fEffects[EFFECT_TAN_DIGITAL_Z_OFFSET])
	} else if (sBit.find("zigzag") != sBit.npos) {
		if (sBit == "zigzag")
			SET_FLOAT(fEffects[EFFECT_ZIGZAG])
		else if (sBit == "zigzagperiod")
			SET_FLOAT(fEffects[EFFECT_ZIGZAG_PERIOD])
		else if (sBit == "zigzagoffset")
			SET_FLOAT(fEffects[EFFECT_ZIGZAG_OFFSET])
		else if (sBit == "zigzagz")
			SET_FLOAT(fEffects[EFFECT_ZIGZAG_Z])
		else if (sBit == "zigzagzperiod")
			SET_FLOAT(fEffects[EFFECT_ZIGZAG_Z_PERIOD])
		else if (sBit == "zigzagzoffset")
			SET_FLOAT(fEffects[EFFECT_ZIGZAG_Z_OFFSET])
	}

	else if (sBit == "blink")
		SET_FLOAT(fAppearances[APPEARANCE_BLINK])
	else if (sBit == "randomvanish")
		SET_FLOAT(fAppearances[APPEARANCE_RANDOMVANISH])
	else if (sBit == "turn" && !on)
		ZERO(m_bTurns); /* "no turn" */
	else if (sBit == "mirror")
		m_bTurns[TURN_MIRROR] = on;
	else if (sBit == "backwards")
		m_bTurns[TURN_BACKWARDS] = on;
	else if (sBit == "left")
		m_bTurns[TURN_LEFT] = on;
	else if (sBit == "right")
		m_bTurns[TURN_RIGHT] = on;
	else if (sBit == "shuffle")
		m_bTurns[TURN_SHUFFLE] = on;
	else if (sBit == "softshuffle")
		m_bTurns[TURN_SOFT_SHUFFLE] = on;
	else if (sBit == "supershuffle")
		m_bTurns[TURN_SUPER_SHUFFLE] = on;
	else if (sBit == "little")
		m_bTransforms[TRANSFORM_LITTLE] = on;
	else if (sBit == "wide")
		m_bTransforms[TRANSFORM_WIDE] = on;
	else if (sBit == "big")
		m_bTransforms[TRANSFORM_BIG] = on;
	else if (sBit == "quick")
		m_bTransforms[TRANSFORM_QUICK] = on;
	else if (sBit == "bmrize")
		m_bTransforms[TRANSFORM_BMRIZE] = on;
	else if (sBit == "skippy")
		m_bTransforms[TRANSFORM_SKIPPY] = on;
	else if (sBit == "mines")
		m_bTransforms[TRANSFORM_MINES] = on;
	else if (sBit == "attackmines")
		m_bTransforms[TRANSFORM_ATTACKMINES] = on;
	else if (sBit == "echo")
		m_bTransforms[TRANSFORM_ECHO] = on;
	else if (sBit == "stomp")
		m_bTransforms[TRANSFORM_STOMP] = on;
	else if (sBit == "jackjs")
		m_bTransforms[TRANSFORM_JACKJS] = on;
	else if (sBit == "anchorjs")
		m_bTransforms[TRANSFORM_ANCHORJS] = on;
	else if (sBit == "icyworld")
		m_bTransforms[TRANSFORM_ICYWORLD] = on;
	else if (sBit == "planted")
		m_bTransforms[TRANSFORM_PLANTED] = on;
	else if (sBit == "floored")
		m_bTransforms[TRANSFORM_FLOORED] = on;
	else if (sBit == "twister")
		m_bTransforms[TRANSFORM_TWISTER] = on;
	else if (sBit == "holdrolls")
		m_bTransforms[TRANSFORM_HOLDROLLS] = on;
	else if (sBit == "nojumps")
		m_bTransforms[TRANSFORM_NOJUMPS] = on;
	else if (sBit == "nohands")
		m_bTransforms[TRANSFORM_NOHANDS] = on;
	else if (sBit == "noquads")
		m_bTransforms[TRANSFORM_NOQUADS] = on;
	else if (sBit.find("reverse") != sBit.npos) {
		if (sBit == "reverse")
			SET_FLOAT(fScrolls[SCROLL_REVERSE])
		else {
			for (int i = 0; i < 16; i++) {
				sMod = ssprintf("reverse%d", i + 1);
				if (sBit == sMod) {
					SET_FLOAT(fReverse[i])
					break;
				}
			}
		}
	} else if (sBit == "split")
		SET_FLOAT(fScrolls[SCROLL_SPLIT])
	else if (sBit == "alternate")
		SET_FLOAT(fScrolls[SCROLL_ALTERNATE])
	else if (sBit == "cross")
		SET_FLOAT(fScrolls[SCROLL_CROSS])
	else if (sBit == "centered")
		SET_FLOAT(fScrolls[SCROLL_CENTERED])
	else if (sBit == "noholds")
		m_bTransforms[TRANSFORM_NOHOLDS] = on;
	else if (sBit == "norolls")
		m_bTransforms[TRANSFORM_NOROLLS] = on;
	else if (sBit == "nomines")
		m_bTransforms[TRANSFORM_NOMINES] = on;
	else if (sBit == "nostretch")
		m_bTransforms[TRANSFORM_NOSTRETCH] = on;
	else if (sBit == "nolifts")
		m_bTransforms[TRANSFORM_NOLIFTS] = on;
	else if (sBit == "nofakes")
		m_bTransforms[TRANSFORM_NOFAKES] = on;
	else if (sBit == "dark")
		SET_FLOAT(fDark)
	else if (sBit == "blind")
		SET_FLOAT(fBlind)
	else if (sBit == "cover")
		SET_FLOAT(fCover)
	else if (sBit == "randomattacks")
		SET_FLOAT(fRandAttack)
	else if (sBit == "noattacks")
		SET_FLOAT(fNoAttack)
	else if (sBit == "playerautoplay")
		SET_FLOAT(fPlayerAutoPlay)
	else if (sBit == "passmark")
		SET_FLOAT(fPassmark)
	else if (sBit == "overhead") {
		m_fSkew = 0;
		m_fPerspectiveTilt = 0;
		m_SpeedfSkew = m_SpeedfPerspectiveTilt = speed;
	} else if (sBit == "incoming") {
		m_fSkew = level;
		m_fPerspectiveTilt = -level;
		m_SpeedfSkew = m_SpeedfPerspectiveTilt = speed;
	} else if (sBit == "space") {
		m_fSkew = level;
		m_fPerspectiveTilt = +level;
		m_SpeedfSkew = m_SpeedfPerspectiveTilt = speed;
	} else if (sBit == "hallway") {
		m_fSkew = 0;
		m_fPerspectiveTilt = -level;
		m_SpeedfSkew = m_SpeedfPerspectiveTilt = speed;
	} else if (sBit == "distant") {
		m_fSkew = 0;
		m_fPerspectiveTilt = +level;
		m_SpeedfSkew = m_SpeedfPerspectiveTilt = speed;
	} else if (NOTESKIN && NOTESKIN->DoesNoteSkinExist(sBit)) {
		m_sNoteSkin = sBit;
	} else if (sBit == "skew")
		SET_FLOAT(fSkew)
	else if (sBit == "tilt")
		SET_FLOAT(fPerspectiveTilt)
	else if (sBit == "noteskin" && !on) /* "no noteskin" */ {
		m_sNoteSkin = CommonMetrics::DEFAULT_NOTESKIN_NAME;
	} else if (sBit == "randomspeed")
		SET_FLOAT(fRandomSpeed)
	else if (sBit == "failarcade" || sBit == "failimmediate")
		m_FailType = FailType_Immediate;
	else if (sBit == "failendofsong" || sBit == "failimmediatecontinue" ||
			 sBit == "failatend")
		m_FailType = FailType_ImmediateContinue;
	else if (sBit == "failoff")
		m_FailType = FailType_Off;
	else if (sBit == "faildefault") {
		PlayerOptions po;
		GAMESTATE->GetDefaultPlayerOptions(po);
		m_FailType = po.m_FailType;
	} else if (sBit == "muteonerror")
		m_bMuteOnError = on;
	else if (sBit == "random")
		ChooseRandomModifiers();
	else if (sBit == "practicemode")
		m_bPractice = on;

	else if (sBit.find("move") != sBit.npos) {
		if (sBit.find("x") != sBit.npos) {
			for (int i = 0; i < 16; i++) {
				sMod = ssprintf("movex%d", i + 1);
				if (sBit == sMod) {
					SET_FLOAT(fMovesX[i])
					break;
				}
			}
		} else if (sBit.find("y") != sBit.npos) {
			for (int i = 0; i < 16; i++) {
				sMod = ssprintf("movey%d", i + 1);
				if (sBit == sMod) {
					SET_FLOAT(fMovesY[i])
					break;
				}
			}
		} else if (sBit.find("z") != sBit.npos) {
			for (int i = 0; i < 16; i++) {
				sMod = ssprintf("movez%d", i + 1);
				if (sBit == sMod) {
					SET_FLOAT(fMovesZ[i])
					break;
				}
			}
		}
	} else if (sBit == "cosecant")
		m_bCosecant = on;
	// deprecated mods/left in for compatibility
	else if (sBit == "converge")
		SET_FLOAT(fScrolls[SCROLL_CENTERED])
	// end of the list
	else {
		return false;
	}

	return true;
}

void
NextFloat(float fValues[], int size)
{
	auto index = -1;
	for (auto i = 0; i < size; i++) {
		if (fValues[i] == 1) {
			index = i;
			break;
		}
	}

	for (auto i = 0; i < size; i++)
		fValues[i] = 0;

	if (index ==
		size - 1) // if true, then the last float in the list was selected
		;		  // leave all off
	else
		fValues[index + 1] = 1;
}

void
NextBool(bool bValues[], int size)
{
	auto index = -1;
	for (auto i = 0; i < size; i++) {
		if (bValues[i]) {
			index = i;
			break;
		}
	}

	for (auto i = 0; i < size; i++)
		bValues[i] = false;

	if (index ==
		size - 1) // if true, then the last float in the list was selected
		;		  // leave all off
	else
		bValues[index + 1] = 1;
}

void
PlayerOptions::NextAccel()
{
	NextFloat(m_fAccels, NUM_ACCELS);
}

void
PlayerOptions::NextEffect()
{
	NextFloat(m_fEffects, NUM_EFFECTS);
}

void
PlayerOptions::NextAppearance()
{
	NextFloat(m_fAppearances, NUM_APPEARANCES);
}

void
PlayerOptions::NextTurn()
{
	NextBool(m_bTurns, NUM_TURNS);
}

void
PlayerOptions::NextTransform()
{
	NextBool(m_bTransforms, NUM_TRANSFORMS);
}

void
PlayerOptions::NextScroll()
{
	NextFloat(m_fScrolls, NUM_SCROLLS);
}

void
PlayerOptions::NextPerspective()
{
	switch ((int)m_fPerspectiveTilt) {
		case -1:
			m_fPerspectiveTilt = 0;
			break;
		case 0:
			m_fPerspectiveTilt = +1;
			break;
		case +1:
		default:
			m_fPerspectiveTilt = -1;
			break;
	}
}

void
PlayerOptions::ChooseRandomModifiers()
{
	if (RandomFloat(0, 1) < RANDOM_SPEED_CHANCE)
		m_fScrollSpeed = 1.5f;
	if (RandomFloat(0, 1) < RANDOM_REVERSE_CHANCE)
		m_fScrolls[SCROLL_REVERSE] = 1;
	if (RandomFloat(0, 1) < RANDOM_DARK_CHANCE)
		m_fDark = 1;
	float f;
	f = RandomFloat(0, 1);
	if (f < RANDOM_ACCEL_CHANCE)
		m_fAccels[RandomInt(NUM_ACCELS)] = 1;
	else if (f < RANDOM_EFFECT_CHANCE)
		m_fEffects[RandomInt(NUM_EFFECTS)] = 1;
	f = RandomFloat(0, 1);
	if (f < RANDOM_HIDDEN_CHANCE)
		m_fAppearances[APPEARANCE_HIDDEN] = 1;
	else if (f < RANDOM_SUDDEN_CHANCE)
		m_fAppearances[APPEARANCE_SUDDEN] = 1;
}

PlayerOptions::Accel
PlayerOptions::GetFirstAccel()
{
	for (auto i = 0; i < NUM_ACCELS; i++)
		if (m_fAccels[i] == 1.f)
			return (Accel)i;
	return (Accel)-1;
}

PlayerOptions::Effect
PlayerOptions::GetFirstEffect()
{
	for (auto i = 0; i < NUM_EFFECTS; i++)
		if (m_fEffects[i] == 1.f)
			return (Effect)i;
	return (Effect)-1;
}

PlayerOptions::Appearance
PlayerOptions::GetFirstAppearance()
{
	for (auto i = 0; i < NUM_APPEARANCES; i++)
		if (m_fAppearances[i] == 1.f)
			return (Appearance)i;
	return (Appearance)-1;
}

PlayerOptions::Scroll
PlayerOptions::GetFirstScroll()
{
	for (auto i = 0; i < NUM_SCROLLS; i++)
		if (m_fScrolls[i] == 1.f)
			return (Scroll)i;
	return (Scroll)-1;
}

void
PlayerOptions::SetOneAccel(Accel a)
{
	ZERO(m_fAccels);
	m_fAccels[a] = 1;
}

void
PlayerOptions::SetOneEffect(Effect e)
{
	ZERO(m_fEffects);
	m_fEffects[e] = 1;
}

void
PlayerOptions::SetOneAppearance(Appearance a)
{
	ZERO(m_fAppearances);
	m_fAppearances[a] = 1;
}

void
PlayerOptions::SetOneScroll(Scroll s)
{
	ZERO(m_fScrolls);
	m_fScrolls[s] = 1;
}

void
PlayerOptions::ToggleOneTurn(Turn t)
{
	const auto bWasOn = m_bTurns[t];
	ZERO(m_bTurns);
	m_bTurns[t] = !bWasOn;
}

float
PlayerOptions::GetReversePercentForColumn(int iCol) const
{
	float f = 0;
	ASSERT(m_pn == PLAYER_1);
	ASSERT(GAMESTATE->GetCurrentStyle(m_pn) != NULL);

	const auto iNumCols = GAMESTATE->GetNumCols(m_pn);

	f += m_fScrolls[SCROLL_REVERSE];
	f += m_fReverse[iCol];

	if (iCol >= iNumCols / 2)
		f += m_fScrolls[SCROLL_SPLIT];
	if ((iCol % 2) == 1)
		f += m_fScrolls[SCROLL_ALTERNATE];

	const auto iFirstCrossCol = iNumCols / 4;
	const auto iLastCrossCol = iNumCols - 1 - iFirstCrossCol;
	if (iCol >= iFirstCrossCol && iCol <= iLastCrossCol)
		f += m_fScrolls[SCROLL_CROSS];

	if (f > 2)
		f = fmodf(f, 2);
	if (f > 1)
		f = SCALE(f, 1.f, 2.f, 1.f, 0.f);
	return f;
}

bool
PlayerOptions::operator==(const PlayerOptions& other) const
{
#define COMPARE(x)                                                             \
	{                                                                          \
		if ((x) != other.x)                                                    \
			return false;                                                      \
	}
	COMPARE(m_LifeType);
	COMPARE(m_DrainType);
	COMPARE(m_BatteryLives);
	COMPARE(m_fTimeSpacing);
	COMPARE(m_fScrollSpeed);
	COMPARE(m_fScrollBPM);
	COMPARE(m_fMaxScrollBPM);
	COMPARE(m_fRandomSpeed);
	COMPARE(m_FailType);
	COMPARE(m_MinTNSToHideNotes);
	COMPARE(m_bMuteOnError);
	COMPARE(m_bCosecant);
	COMPARE(m_bStealthType);
	COMPARE(m_bStealthPastReceptors);
	COMPARE(m_fDark);
	COMPARE(m_fBlind);
	COMPARE(m_fCover);
	COMPARE(m_fRandAttack);
	COMPARE(m_fNoAttack);
	COMPARE(m_fPlayerAutoPlay);
	COMPARE(m_fPerspectiveTilt);
	COMPARE(m_fSkew);
	// The noteskin name needs to be compared case-insensitively because the
	// manager forces lowercase, but some obscure part of PlayerOptions
	// uppercases the first letter.  The previous code that used != probably
	// relied on std::string::operator!= misbehaving. -Kyz
	if (strcasecmp(m_sNoteSkin.c_str(), other.m_sNoteSkin.c_str()) != 0) {
		return false;
	}
	for (auto i = 0; i < PlayerOptions::NUM_ACCELS; ++i)
		COMPARE(m_fAccels[i]);
	for (auto i = 0; i < PlayerOptions::NUM_EFFECTS; ++i)
		COMPARE(m_fEffects[i]);
	for (auto i = 0; i < PlayerOptions::NUM_APPEARANCES; ++i)
		COMPARE(m_fAppearances[i]);
	for (auto i = 0; i < PlayerOptions::NUM_SCROLLS; ++i)
		COMPARE(m_fScrolls[i]);
	for (auto i = 0; i < PlayerOptions::NUM_TURNS; ++i)
		COMPARE(m_bTurns[i]);
	for (auto i = 0; i < PlayerOptions::NUM_TRANSFORMS; ++i)
		COMPARE(m_bTransforms[i]);
	for (int i = 0; i < 16; ++i)
		COMPARE(m_fMovesX[i]);
	for (int i = 0; i < 16; ++i)
		COMPARE(m_fMovesY[i]);
	for (int i = 0; i < 16; ++i)
		COMPARE(m_fMovesZ[i]);
	for (int i = 0; i < 16; ++i)
		COMPARE(m_fBumpy[i]);
	for (int i = 0; i < 16; ++i)
		COMPARE(m_fTiny[i]);
	for (int i = 0; i < 16; ++i)
		COMPARE(m_fConfusionX[i]);
	for (int i = 0; i < 16; ++i)
		COMPARE(m_fConfusionY[i]);
	for (int i = 0; i < 16; ++i)
		COMPARE(m_fConfusionZ[i]);
	for (int i = 0; i < 16; ++i)
		COMPARE(m_fStealth[i]);
	for (int i = 0; i < 16; ++i)
		COMPARE(m_fReverse[i]);
#undef COMPARE
	return true;
}

PlayerOptions&
PlayerOptions::operator=(PlayerOptions const& other)
{
// Do not copy m_pn from the other, it must be preserved as what PlayerState set
// it to.
#define CPY(x) x = other.x;
#define CPY_SPEED(x)                                                           \
	m_##x = other.m_##x;                                                       \
	m_Speed##x = other.m_Speed##x;
	CPY(m_LifeType);
	CPY(m_DrainType);
	CPY(m_BatteryLives);
	CPY_SPEED(fTimeSpacing);
	CPY_SPEED(fScrollSpeed);
	CPY_SPEED(fScrollBPM);
	CPY_SPEED(fMaxScrollBPM);
	CPY_SPEED(fRandomSpeed);
	CPY(m_FailType);
	CPY(m_MinTNSToHideNotes);
	CPY(m_bMuteOnError);
	CPY(m_bCosecant);
	CPY(m_bStealthType);
	CPY(m_bStealthPastReceptors);
	CPY_SPEED(fDark);
	CPY_SPEED(fBlind);
	CPY_SPEED(fCover);
	CPY_SPEED(fRandAttack);
	CPY_SPEED(fNoAttack);
	CPY_SPEED(fPlayerAutoPlay);
	CPY_SPEED(fPerspectiveTilt);
	CPY_SPEED(fSkew);
	if (!other.m_sNoteSkin.empty() &&
		NOTESKIN->DoesNoteSkinExist(other.m_sNoteSkin)) {
		CPY(m_sNoteSkin);
	}
	for (auto i = 0; i < PlayerOptions::NUM_ACCELS; ++i) {
		CPY_SPEED(fAccels[i]);
	}
	for (auto i = 0; i < PlayerOptions::NUM_EFFECTS; ++i) {
		CPY_SPEED(fEffects[i]);
	}
	for (auto i = 0; i < PlayerOptions::NUM_APPEARANCES; ++i) {
		CPY_SPEED(fAppearances[i]);
	}
	for (auto i = 0; i < PlayerOptions::NUM_SCROLLS; ++i) {
		CPY_SPEED(fScrolls[i]);
	}
	for (auto i = 0; i < PlayerOptions::NUM_TURNS; ++i) {
		CPY(m_bTurns[i]);
	}
	for (auto i = 0; i < PlayerOptions::NUM_TRANSFORMS; ++i) {
		CPY(m_bTransforms[i]);
	}
	for (int i = 0; i < 16; ++i) {
		CPY_SPEED(fMovesX[i]);
	}
	for (int i = 0; i < 16; ++i) {
		CPY_SPEED(fMovesY[i]);
	}
	for (int i = 0; i < 16; ++i) {
		CPY_SPEED(fMovesZ[i]);
	}
	for (int i = 0; i < 16; ++i) {
		CPY_SPEED(fBumpy[i]);
	}
	for (int i = 0; i < 16; ++i) {
		CPY_SPEED(fTiny[i]);
	}
	for (int i = 0; i < 16; ++i) {
		CPY_SPEED(fConfusionX[i]);
	}
	for (int i = 0; i < 16; ++i) {
		CPY_SPEED(fConfusionY[i]);
	}
	for (int i = 0; i < 16; ++i) {
		CPY_SPEED(fConfusionZ[i]);
	}
	for (int i = 0; i < 16; ++i) {
		CPY_SPEED(fStealth[i]);
	}
	for (int i = 0; i < 16; ++i) {
		CPY_SPEED(fReverse[i]);
	}
#undef CPY
#undef CPY_SPEED
	return *this;
}

bool
PlayerOptions::IsEasierForSongAndSteps(Song* pSong,
									   Steps* pSteps,
									   PlayerNumber pn) const
{
	if (m_fTimeSpacing && pSteps->HasSignificantTimingChanges())
		return true;
	const auto& rv = pSteps->GetRadarValues();
	if (m_bTransforms[TRANSFORM_NOHOLDS] && rv[RadarCategory_Holds] > 0)
		return true;
	if (m_bTransforms[TRANSFORM_NOROLLS] && rv[RadarCategory_Rolls] > 0)
		return true;
	if (m_bTransforms[TRANSFORM_NOMINES] && rv[RadarCategory_Mines] > 0)
		return true;
	if (m_bTransforms[TRANSFORM_NOHANDS] && rv[RadarCategory_Hands] > 0)
		return true;
	if (m_bTransforms[TRANSFORM_NOQUADS] && rv[RadarCategory_Hands] > 0)
		return true;
	if (m_bTransforms[TRANSFORM_NOJUMPS] && rv[RadarCategory_Jumps] > 0)
		return true;
	if (m_bTransforms[TRANSFORM_NOLIFTS] && rv[RadarCategory_Lifts] > 0)
		return true;
	if (m_bTransforms[TRANSFORM_NOFAKES] && rv[RadarCategory_Fakes] > 0)
		return true;
	if (m_bTransforms[TRANSFORM_NOSTRETCH])
		return true;

	// Inserted holds can be really easy on some songs, and scores will be
	// highly hold-weighted, and very little tap score weighted.
	if (m_bTransforms[TRANSFORM_LITTLE])
		return true;
	if (m_bTransforms[TRANSFORM_PLANTED])
		return true;
	if (m_bTransforms[TRANSFORM_FLOORED])
		return true;
	if (m_bTransforms[TRANSFORM_TWISTER])
		return true;

	// This makes songs with sparse notes easier.
	if (m_bTransforms[TRANSFORM_ECHO])
		return true;

	if (m_fCover)
		return true;

	// M-mods make songs with indefinite BPMs easier because
	// they ensure that the song has a scrollable speed.
	if (m_fMaxScrollBPM != 0) {
		// BPM display is obfuscated
		//		if( pSong->m_DisplayBPMType == DISPLAY_BPM_RANDOM )
		//			return true;

		DisplayBpms bpms;
		GAMESTATE->m_pCurSong->GetDisplayBpms(bpms);
		pSong->GetDisplayBpms(bpms);

		// maximum BPM is obfuscated, so M-mods will set a playable speed.
		if (bpms.GetMax() <= 0)
			return true;
	}
	if (m_fPlayerAutoPlay)
		return true;
	return false;
}

void
PlayerOptions::GetLocalizedMods(vector<std::string>& AddTo) const
{
	vector<std::string> vMods;
	GetMods(vMods);
	for (auto& s : vMods) {
		const auto& sOneMod = s;

		ASSERT(!sOneMod.empty());

		vector<std::string> asTokens;
		split(sOneMod, " ", asTokens);

		if (asTokens.empty())
			continue;

		// Strip the approach speed token, if any
		if (asTokens[0][0] == '*')
			asTokens.erase(asTokens.begin());

		// capitalize NoteSkin names
		asTokens.back() = Capitalize(asTokens.back());

		/* Theme the mod name (the last string).  Allow this to not exist, since
		 * characters might use modifiers that don't exist in the theme. */
		asTokens.back() =
		  CommonMetrics::LocalizeOptionItem(asTokens.back(), true);

		auto sLocalizedMod = join(" ", asTokens);
		AddTo.push_back(sLocalizedMod);
	}
}

bool
PlayerOptions::ContainsTransformOrTurn() const
{
	for (auto i = 0; i < NUM_TRANSFORMS; i++) {
		switch (i) {
			case TRANSFORM_NOHOLDS:
			case TRANSFORM_NOROLLS:
			case TRANSFORM_NOMINES:
			case TRANSFORM_ATTACKMINES:
				continue;
		}
		if (m_bTransforms[i])
			return true;
	}
	for (auto i = 0; i < NUM_TURNS; i++) {
		if (m_bTurns[i] && i != TURN_MIRROR)
			return true;
	}
	return false;
}

vector<std::string>
PlayerOptions::GetInvalidatingModifiers() const
{
	vector<std::string> AddTo;

	if (m_bTurns[TURN_BACKWARDS])
		AddTo.push_back("Backwards");
	if (m_bTurns[TURN_LEFT])
		AddTo.push_back("Left");
	if (m_bTurns[TURN_RIGHT])
		AddTo.push_back("Right");
	if (m_bTurns[TURN_SHUFFLE])
		AddTo.push_back("Shuffle");
	if (m_bTurns[TURN_SOFT_SHUFFLE])
		AddTo.push_back("SoftShuffle");
	if (m_bTurns[TURN_SUPER_SHUFFLE])
		AddTo.push_back("SuperShuffle");

	if (m_bTransforms[TRANSFORM_NOHOLDS])
		AddTo.push_back("NoHolds");
	if (m_bTransforms[TRANSFORM_NOROLLS])
		AddTo.push_back("NoRolls");
	if (m_bTransforms[TRANSFORM_NOMINES])
		AddTo.push_back("NoMines");
	if (m_bTransforms[TRANSFORM_MINES])
		AddTo.push_back("Mines");
	if (m_bTransforms[TRANSFORM_LITTLE])
		AddTo.push_back("Little");
	if (m_bTransforms[TRANSFORM_NOJUMPS])
		AddTo.push_back("NoJumps");
	if (m_bTransforms[TRANSFORM_NOHANDS])
		AddTo.push_back("NoHands");
	if (m_bTransforms[TRANSFORM_NOLIFTS])
		AddTo.push_back("NoLifts");
	if (m_bTransforms[TRANSFORM_NOFAKES])
		AddTo.push_back("NoFakes");
	if (m_bTransforms[TRANSFORM_NOQUADS])
		AddTo.push_back("NoQuads");
	if (m_bTransforms[TRANSFORM_NOSTRETCH])
		AddTo.push_back("NoStretch");

	return AddTo;
}

std::string
PlayerOptions::GetSavedPrefsString() const
{
	PlayerOptions po_prefs;
#define SAVE(x) po_prefs.x = this->x;
	SAVE(m_fTimeSpacing);
	SAVE(m_fScrollSpeed);
	SAVE(m_fScrollBPM);
	SAVE(m_fMaxScrollBPM);
	SAVE(m_fScrolls[SCROLL_REVERSE]);
	SAVE(m_fPerspectiveTilt);
	SAVE(m_bTransforms[TRANSFORM_NOHOLDS]);
	SAVE(m_bTransforms[TRANSFORM_NOROLLS]);
	SAVE(m_bTransforms[TRANSFORM_NOMINES]);
	SAVE(m_bTransforms[TRANSFORM_NOJUMPS]);
	SAVE(m_bTransforms[TRANSFORM_NOHANDS]);
	SAVE(m_bTransforms[TRANSFORM_NOQUADS]);
	SAVE(m_bTransforms[TRANSFORM_NOSTRETCH]);
	SAVE(m_bTransforms[TRANSFORM_NOLIFTS]);
	SAVE(m_bTransforms[TRANSFORM_NOFAKES]);
	SAVE(m_bMuteOnError);
	SAVE(m_sNoteSkin);
	SAVE(m_fAppearances[APPEARANCE_HIDDEN]);
	SAVE(m_fAppearances[APPEARANCE_HIDDEN_OFFSET]);
	SAVE(m_fAppearances[APPEARANCE_SUDDEN]);
	SAVE(m_fAppearances[APPEARANCE_SUDDEN_OFFSET]);
	SAVE(m_fAppearances[APPEARANCE_STEALTH]);
	SAVE(m_fAppearances[APPEARANCE_BLINK]);
	SAVE(m_fAppearances[APPEARANCE_RANDOMVANISH]);
	SAVE(m_fDark);
	SAVE(m_fBlind);
	SAVE(m_fCover);
#undef SAVE
	return po_prefs.GetString();
}

void
PlayerOptions::ResetPrefs(ResetPrefsType type)
{
	PlayerOptions defaults;
#define CPY(x) this->x = defaults.x;
	switch (type) {
		DEFAULT_FAIL(type);
		case saved_prefs:
			CPY(m_fTimeSpacing);
			CPY(m_fScrollSpeed);
			CPY(m_fScrollBPM);
			CPY(m_fMaxScrollBPM);
			break;
	}
	CPY(m_LifeType);
	CPY(m_DrainType);
	CPY(m_BatteryLives);
	CPY(m_MinTNSToHideNotes);
	CPY(m_fPerspectiveTilt);

	CPY(m_bTransforms[TRANSFORM_NOHOLDS]);
	CPY(m_bTransforms[TRANSFORM_NOROLLS]);
	CPY(m_bTransforms[TRANSFORM_NOMINES]);
	CPY(m_bTransforms[TRANSFORM_NOJUMPS]);
	CPY(m_bTransforms[TRANSFORM_NOHANDS]);
	CPY(m_bTransforms[TRANSFORM_NOQUADS]);
	CPY(m_bTransforms[TRANSFORM_NOSTRETCH]);
	CPY(m_bTransforms[TRANSFORM_NOLIFTS]);
	CPY(m_bTransforms[TRANSFORM_NOFAKES]);

	CPY(m_fAppearances[APPEARANCE_HIDDEN]);
	CPY(m_fAppearances[APPEARANCE_HIDDEN_OFFSET]);
	CPY(m_fAppearances[APPEARANCE_SUDDEN]);
	CPY(m_fAppearances[APPEARANCE_SUDDEN_OFFSET]);
	CPY(m_fAppearances[APPEARANCE_STEALTH]);
	CPY(m_fAppearances[APPEARANCE_BLINK]);
	CPY(m_fAppearances[APPEARANCE_RANDOMVANISH]);
	CPY(m_fDark);
	CPY(m_fBlind);
	CPY(m_fCover);
	CPY(m_bStealthType);
	CPY(m_bStealthPastReceptors);
	// Don't clear this.
	// CPY( m_sNoteSkin );
#undef CPY
}

// lua start
#include "Etterna/Models/Lua/LuaBinding.h"
#include "Etterna/Globals/OptionsBinding.h"

/** @brief Allow Lua to have access to PlayerOptions. */
class LunaPlayerOptions : public Luna<PlayerOptions>
{
  public:
	static int IsEasierForSongAndSteps(T* p, lua_State* L)
	{
		auto pSong = Luna<Song>::check(L, 1);
		auto pSteps = Luna<Steps>::check(L, 2);
		const auto pn = Enum::Check<PlayerNumber>(L, 3);
		lua_pushboolean(L, p->IsEasierForSongAndSteps(pSong, pSteps, pn));
		return 1;
	}
	// Direct control functions, for themes that can handle it.

	ENUM_INTERFACE(LifeSetting, LifeType, LifeType);
	ENUM_INTERFACE(DrainSetting, DrainType, DrainType);
	INT_INTERFACE(BatteryLives, BatteryLives);
	FLOAT_INTERFACE(TimeSpacing, TimeSpacing, true);
	FLOAT_INTERFACE(MaxScrollBPM, MaxScrollBPM, true);
	FLOAT_INTERFACE(ScrollSpeed, ScrollSpeed, true);
	FLOAT_INTERFACE(ScrollBPM, ScrollBPM, true);

	FLOAT_INTERFACE(Boost, Accels[PlayerOptions::ACCEL_BOOST], true);
	FLOAT_INTERFACE(Brake, Accels[PlayerOptions::ACCEL_BRAKE], true);
	FLOAT_INTERFACE(Wave, Accels[PlayerOptions::ACCEL_WAVE], true);
	FLOAT_INTERFACE(WavePeriod, Accels[PlayerOptions::ACCEL_WAVE_PERIOD], true);
	FLOAT_INTERFACE(Expand, Accels[PlayerOptions::ACCEL_EXPAND], true);
	FLOAT_INTERFACE(ExpandPeriod,
					Accels[PlayerOptions::ACCEL_EXPAND_PERIOD],
					true);
	FLOAT_INTERFACE(TanExpand, Accels[PlayerOptions::ACCEL_TAN_EXPAND], true);
	FLOAT_INTERFACE(TanExpandPeriod,
					Accels[PlayerOptions::ACCEL_TAN_EXPAND_PERIOD],
					true);
	FLOAT_INTERFACE(Boomerang, Accels[PlayerOptions::ACCEL_BOOMERANG], true);

	FLOAT_INTERFACE(Drunk, Effects[PlayerOptions::EFFECT_DRUNK], true);
	FLOAT_INTERFACE(Dizzy, Effects[PlayerOptions::EFFECT_DIZZY], true);
	FLOAT_INTERFACE(Confusion, Effects[PlayerOptions::EFFECT_CONFUSION], true);
	FLOAT_INTERFACE(ConfusionOffset,
					Effects[PlayerOptions::EFFECT_CONFUSION_OFFSET],
					true);
	FLOAT_INTERFACE(ConfusionX,
					Effects[PlayerOptions::EFFECT_CONFUSION_X],
					true);
	FLOAT_INTERFACE(ConfusionXOffset,
					Effects[PlayerOptions::EFFECT_CONFUSION_X_OFFSET],
					true);
	FLOAT_INTERFACE(ConfusionY,
					Effects[PlayerOptions::EFFECT_CONFUSION_Y],
					true);
	FLOAT_INTERFACE(ConfusionYOffset,
					Effects[PlayerOptions::EFFECT_CONFUSION_Y_OFFSET],
					true);
	FLOAT_INTERFACE(Mini, Effects[PlayerOptions::EFFECT_MINI], true);
	FLOAT_INTERFACE(Tiny, Effects[PlayerOptions::EFFECT_TINY], true);
	FLOAT_INTERFACE(Flip, Effects[PlayerOptions::EFFECT_FLIP], true);
	FLOAT_INTERFACE(Invert, Effects[PlayerOptions::EFFECT_INVERT], true);
	FLOAT_INTERFACE(Tornado, Effects[PlayerOptions::EFFECT_TORNADO], true);
	FLOAT_INTERFACE(Tipsy, Effects[PlayerOptions::EFFECT_TIPSY], true);
	FLOAT_INTERFACE(Bumpy, Effects[PlayerOptions::EFFECT_BUMPY], true);
	FLOAT_INTERFACE(Beat, Effects[PlayerOptions::EFFECT_BEAT], true);
	FLOAT_INTERFACE(Xmode, Effects[PlayerOptions::EFFECT_XMODE], true);
	FLOAT_INTERFACE(Twirl, Effects[PlayerOptions::EFFECT_TWIRL], true);
	FLOAT_INTERFACE(Roll, Effects[PlayerOptions::EFFECT_ROLL], true);
	FLOAT_INTERFACE(AttenuateX,
					Effects[PlayerOptions::EFFECT_ATTENUATE_X],
					true);
	FLOAT_INTERFACE(AttenuateY,
					Effects[PlayerOptions::EFFECT_ATTENUATE_Y],
					true);
	FLOAT_INTERFACE(AttenuateZ,
					Effects[PlayerOptions::EFFECT_ATTENUATE_Z],
					true);
	FLOAT_INTERFACE(Bounce, Effects[PlayerOptions::EFFECT_BOUNCE], true);
	FLOAT_INTERFACE(BouncePeriod,
					Effects[PlayerOptions::EFFECT_BOUNCE_PERIOD],
					true);
	FLOAT_INTERFACE(BounceOffset,
					Effects[PlayerOptions::EFFECT_BOUNCE_OFFSET],
					true);
	FLOAT_INTERFACE(BounceZ, Effects[PlayerOptions::EFFECT_BOUNCE_Z], true);
	FLOAT_INTERFACE(BounceZPeriod,
					Effects[PlayerOptions::EFFECT_BOUNCE_Z_PERIOD],
					true);
	FLOAT_INTERFACE(BounceZOffset,
					Effects[PlayerOptions::EFFECT_BOUNCE_Z_OFFSET],
					true);
	FLOAT_INTERFACE(Digital, Effects[PlayerOptions::EFFECT_DIGITAL], true);
	FLOAT_INTERFACE(DigitalSteps,
					Effects[PlayerOptions::EFFECT_DIGITAL_STEPS],
					true);
	FLOAT_INTERFACE(DigitalPeriod,
					Effects[PlayerOptions::EFFECT_DIGITAL_PERIOD],
					true);
	FLOAT_INTERFACE(DigitalOffset,
					Effects[PlayerOptions::EFFECT_DIGITAL_OFFSET],
					true);
	FLOAT_INTERFACE(TanDigital,
					Effects[PlayerOptions::EFFECT_TAN_DIGITAL],
					true);
	FLOAT_INTERFACE(TanDigitalSteps,
					Effects[PlayerOptions::EFFECT_TAN_DIGITAL_STEPS],
					true);
	FLOAT_INTERFACE(TanDigitalPeriod,
					Effects[PlayerOptions::EFFECT_TAN_DIGITAL_PERIOD],
					true);
	FLOAT_INTERFACE(TanDigitalOffset,
					Effects[PlayerOptions::EFFECT_TAN_DIGITAL_OFFSET],
					true);
	FLOAT_INTERFACE(DigitalZ, Effects[PlayerOptions::EFFECT_DIGITAL_Z], true);
	FLOAT_INTERFACE(DigitalZSteps,
					Effects[PlayerOptions::EFFECT_DIGITAL_Z_STEPS],
					true);
	FLOAT_INTERFACE(DigitalZPeriod,
					Effects[PlayerOptions::EFFECT_DIGITAL_Z_PERIOD],
					true);
	FLOAT_INTERFACE(DigitalZOffset,
					Effects[PlayerOptions::EFFECT_DIGITAL_Z_OFFSET],
					true);
	FLOAT_INTERFACE(TanDigitalZ,
					Effects[PlayerOptions::EFFECT_TAN_DIGITAL_Z],
					true);
	FLOAT_INTERFACE(TanDigitalZSteps,
					Effects[PlayerOptions::EFFECT_TAN_DIGITAL_Z_STEPS],
					true);
	FLOAT_INTERFACE(TanDigitalZPeriod,
					Effects[PlayerOptions::EFFECT_TAN_DIGITAL_Z_PERIOD],
					true);
	FLOAT_INTERFACE(TanDigitalZOffset,
					Effects[PlayerOptions::EFFECT_TAN_DIGITAL_Z_OFFSET],
					true);
	FLOAT_INTERFACE(Zigzag, Effects[PlayerOptions::EFFECT_ZIGZAG], true);
	FLOAT_INTERFACE(ZigzagPeriod,
					Effects[PlayerOptions::EFFECT_ZIGZAG_PERIOD],
					true);
	FLOAT_INTERFACE(ZigzagOffset,
					Effects[PlayerOptions::EFFECT_ZIGZAG_OFFSET],
					true);
	FLOAT_INTERFACE(ZigzagZ, Effects[PlayerOptions::EFFECT_ZIGZAG_Z], true);
	FLOAT_INTERFACE(ZigzagZPeriod,
					Effects[PlayerOptions::EFFECT_ZIGZAG_Z_PERIOD],
					true);
	FLOAT_INTERFACE(ZigzagZOffset,
					Effects[PlayerOptions::EFFECT_ZIGZAG_Z_OFFSET],
					true);

	FLOAT_INTERFACE(Hidden,
					Appearances[PlayerOptions::APPEARANCE_HIDDEN],
					true);
	FLOAT_INTERFACE(HiddenOffset,
					Appearances[PlayerOptions::APPEARANCE_HIDDEN_OFFSET],
					true);
	FLOAT_INTERFACE(Sudden,
					Appearances[PlayerOptions::APPEARANCE_SUDDEN],
					true);
	FLOAT_INTERFACE(SuddenOffset,
					Appearances[PlayerOptions::APPEARANCE_SUDDEN_OFFSET],
					true);
	FLOAT_INTERFACE(Stealth,
					Appearances[PlayerOptions::APPEARANCE_STEALTH],
					true);
	FLOAT_INTERFACE(Blink, Appearances[PlayerOptions::APPEARANCE_BLINK], true);
	FLOAT_INTERFACE(RandomVanish,
					Appearances[PlayerOptions::APPEARANCE_RANDOMVANISH],
					true);

	FLOAT_INTERFACE(Reverse, Scrolls[PlayerOptions::SCROLL_REVERSE], true);
	FLOAT_INTERFACE(Split, Scrolls[PlayerOptions::SCROLL_SPLIT], true);
	FLOAT_INTERFACE(Alternate, Scrolls[PlayerOptions::SCROLL_ALTERNATE], true);
	FLOAT_INTERFACE(Cross, Scrolls[PlayerOptions::SCROLL_CROSS], true);
	FLOAT_INTERFACE(Centered, Scrolls[PlayerOptions::SCROLL_CENTERED], true);

	FLOAT_INTERFACE(Dark, Dark, true);
	FLOAT_INTERFACE(Blind, Blind, true);
	FLOAT_INTERFACE(Cover, Cover, true);

	FLOAT_INTERFACE(RandAttack, RandAttack, true);
	FLOAT_INTERFACE(NoAttack, NoAttack, true);
	FLOAT_INTERFACE(PlayerAutoPlay, PlayerAutoPlay, true);
	FLOAT_INTERFACE(Skew, Skew, true);
	FLOAT_INTERFACE(Tilt, PerspectiveTilt, true);
	FLOAT_INTERFACE(Passmark, Passmark, true); // Passmark is not sanity checked
											   // to the [0, 1] range because
											   // LifeMeterBar::IsFailing is the
											   // only thing that uses it, and
											   // it's used in a <= test.  Any
											   // theme passing a value outside
											   // the [0, 1] range probably
											   // expects the result they get.
											   // -Kyz
	FLOAT_INTERFACE(RandomSpeed, RandomSpeed, true);

	BOOL_INTERFACE(Cosecant, Cosecant);
	BOOL_INTERFACE(StealthType, StealthType);
	BOOL_INTERFACE(StealthPastReceptors, StealthPastReceptors);

	MULTICOL_FLOAT_INTERFACE(MoveX, MovesX, true);
	MULTICOL_FLOAT_INTERFACE(MoveY, MovesY, true);
	MULTICOL_FLOAT_INTERFACE(MoveZ, MovesZ, true);
	MULTICOL_FLOAT_INTERFACE(Bumpy, Bumpy, true);
	MULTICOL_FLOAT_INTERFACE(Tiny, Tiny, true);
	MULTICOL_FLOAT_INTERFACE(ConfusionXOffset, ConfusionX, true);
	MULTICOL_FLOAT_INTERFACE(ConfusionYOffset, ConfusionY, true);
	MULTICOL_FLOAT_INTERFACE(ConfusionOffset, ConfusionZ, true);
	MULTICOL_FLOAT_INTERFACE(Stealth, Stealth, true);
	MULTICOL_FLOAT_INTERFACE(Reverse, Reverse, true);

	BOOL_INTERFACE(TurnNone, Turns[PlayerOptions::TURN_NONE]);
	BOOL_INTERFACE(Mirror, Turns[PlayerOptions::TURN_MIRROR]);
	BOOL_INTERFACE(Backwards, Turns[PlayerOptions::TURN_BACKWARDS]);
	BOOL_INTERFACE(Left, Turns[PlayerOptions::TURN_LEFT]);
	BOOL_INTERFACE(Right, Turns[PlayerOptions::TURN_RIGHT]);
	BOOL_INTERFACE(Shuffle, Turns[PlayerOptions::TURN_SHUFFLE]);
	BOOL_INTERFACE(SoftShuffle, Turns[PlayerOptions::TURN_SOFT_SHUFFLE]);
	BOOL_INTERFACE(SuperShuffle, Turns[PlayerOptions::TURN_SUPER_SHUFFLE]);
	BOOL_INTERFACE(NoHolds, Transforms[PlayerOptions::TRANSFORM_NOHOLDS]);
	BOOL_INTERFACE(NoRolls, Transforms[PlayerOptions::TRANSFORM_NOROLLS]);
	BOOL_INTERFACE(NoMines, Transforms[PlayerOptions::TRANSFORM_NOMINES]);
	BOOL_INTERFACE(Little, Transforms[PlayerOptions::TRANSFORM_LITTLE]);
	BOOL_INTERFACE(Wide, Transforms[PlayerOptions::TRANSFORM_WIDE]);
	BOOL_INTERFACE(Big, Transforms[PlayerOptions::TRANSFORM_BIG]);
	BOOL_INTERFACE(Quick, Transforms[PlayerOptions::TRANSFORM_QUICK]);
	BOOL_INTERFACE(BMRize, Transforms[PlayerOptions::TRANSFORM_BMRIZE]);
	BOOL_INTERFACE(Skippy, Transforms[PlayerOptions::TRANSFORM_SKIPPY]);
	BOOL_INTERFACE(Mines, Transforms[PlayerOptions::TRANSFORM_MINES]);
	BOOL_INTERFACE(AttackMines,
				   Transforms[PlayerOptions::TRANSFORM_ATTACKMINES]);
	BOOL_INTERFACE(Echo, Transforms[PlayerOptions::TRANSFORM_ECHO]);
	BOOL_INTERFACE(Stomp, Transforms[PlayerOptions::TRANSFORM_STOMP]);
	BOOL_INTERFACE(JackJS, Transforms[PlayerOptions::TRANSFORM_JACKJS]);
	BOOL_INTERFACE(AnchorJS, Transforms[PlayerOptions::TRANSFORM_ANCHORJS]);
	BOOL_INTERFACE(IcyWorld, Transforms[PlayerOptions::TRANSFORM_ICYWORLD]);
	BOOL_INTERFACE(Planted, Transforms[PlayerOptions::TRANSFORM_PLANTED]);
	BOOL_INTERFACE(Floored, Transforms[PlayerOptions::TRANSFORM_FLOORED]);
	BOOL_INTERFACE(Twister, Transforms[PlayerOptions::TRANSFORM_TWISTER]);
	BOOL_INTERFACE(HoldRolls, Transforms[PlayerOptions::TRANSFORM_HOLDROLLS]);
	BOOL_INTERFACE(NoJumps, Transforms[PlayerOptions::TRANSFORM_NOJUMPS]);
	BOOL_INTERFACE(NoHands, Transforms[PlayerOptions::TRANSFORM_NOHANDS]);
	BOOL_INTERFACE(NoLifts, Transforms[PlayerOptions::TRANSFORM_NOLIFTS]);
	BOOL_INTERFACE(NoFakes, Transforms[PlayerOptions::TRANSFORM_NOFAKES]);
	BOOL_INTERFACE(NoQuads, Transforms[PlayerOptions::TRANSFORM_NOQUADS]);
	BOOL_INTERFACE(NoStretch, Transforms[PlayerOptions::TRANSFORM_NOSTRETCH]);

	BOOL_INTERFACE(MuteOnError, MuteOnError);
	BOOL_INTERFACE(PracticeMode, Practice)
	ENUM_INTERFACE(FailSetting, FailType, FailType);
	ENUM_INTERFACE(MinTNSToHideNotes, MinTNSToHideNotes, TapNoteScore);

	// NoteSkins
	static int NoteSkin(T* p, lua_State* L)
	{
		const auto original_top = lua_gettop(L);
		if (p->m_sNoteSkin.empty()) {
			lua_pushstring(
			  L, CommonMetrics::DEFAULT_NOTESKIN_NAME.GetValue().c_str());
		} else {
			lua_pushstring(L, p->m_sNoteSkin.c_str());
		}
		if (original_top >= 1 && (lua_isstring(L, 1) != 0)) {
			const std::string skin = SArg(1);
			if (NOTESKIN->DoesNoteSkinExist(skin)) {
				p->m_sNoteSkin = skin;
				lua_pushboolean(L, 1);
			} else {
				lua_pushnil(L);
			}
		} else {
			lua_pushnil(L);
		}
		OPTIONAL_RETURN_SELF(original_top);
		return 2;
	}

	static void SetSpeedModApproaches(T* p, float speed)
	{
		p->m_SpeedfScrollBPM = speed;
		p->m_SpeedfScrollSpeed = speed;
		p->m_SpeedfMaxScrollBPM = speed;
		p->m_SpeedfTimeSpacing = speed;
	}

	// Speed Mods
	// Sanity checked functions for speed mods, for themes that want to use the
	// engine's enforcement of sane separation between speed mod types.
	static int CMod(T* p, lua_State* L)
	{
		const auto original_top = lua_gettop(L);
		if (p->m_fTimeSpacing) {
			lua_pushnumber(L, p->m_fScrollBPM);
			lua_pushnumber(L, p->m_SpeedfScrollBPM);
		} else {
			lua_pushnil(L);
			lua_pushnil(L);
		}
		if (original_top >= 1 && (lua_isnumber(L, 1) != 0)) {
			const auto speed = FArg(1);
			if (!std::isfinite(speed) || speed <= 0.0f) {
				luaL_error(L, "CMod speed must be finite and greater than 0.");
			}
			p->m_fScrollBPM = speed;
			p->m_fTimeSpacing = 1;
			p->m_fScrollSpeed = 1;
			p->m_fMaxScrollBPM = 0;
		}
		if (original_top >= 2 && (lua_isnumber(L, 2) != 0)) {
			SetSpeedModApproaches(p, FArgGTEZero(L, 2));
		}
		OPTIONAL_RETURN_SELF(original_top);
		return 2;
	}

	static int XMod(T* p, lua_State* L)
	{
		const auto original_top = lua_gettop(L);
		if (!p->m_fTimeSpacing) {
			lua_pushnumber(L, p->m_fScrollSpeed);
			lua_pushnumber(L, p->m_SpeedfScrollSpeed);
		} else {
			lua_pushnil(L);
			lua_pushnil(L);
		}
		if ((lua_isnumber(L, 1) != 0) && original_top >= 1) {
			p->m_fScrollSpeed = FArg(1);
			p->m_fTimeSpacing = 0;
			p->m_fScrollBPM = CMOD_DEFAULT;
			p->m_fMaxScrollBPM = 0;
		}
		if ((lua_isnumber(L, 2) != 0) && original_top >= 2) {
			SetSpeedModApproaches(p, FArgGTEZero(L, 2));
		}
		OPTIONAL_RETURN_SELF(original_top);
		return 2;
	}

	static int MMod(T* p, lua_State* L)
	{
		const auto original_top = lua_gettop(L);
		if (!p->m_fTimeSpacing && p->m_fMaxScrollBPM) {
			lua_pushnumber(L, p->m_fMaxScrollBPM);
			lua_pushnumber(L, p->m_SpeedfMaxScrollBPM);
		} else {
			lua_pushnil(L);
			lua_pushnil(L);
		}
		if ((lua_isnumber(L, 1) != 0) && original_top >= 1) {
			const auto speed = FArg(1);
			if (!std::isfinite(speed) || speed <= 0.0f) {
				luaL_error(L, "MMod speed must be finite and greater than 0.");
			}
			p->m_fScrollBPM = CMOD_DEFAULT;
			p->m_fTimeSpacing = 0;
			p->m_fScrollSpeed = 1;
			p->m_fMaxScrollBPM = speed;
		}
		if ((lua_isnumber(L, 2) != 0) && original_top >= 2) {
			SetSpeedModApproaches(p, FArgGTEZero(L, 2));
		}
		OPTIONAL_RETURN_SELF(original_top);
		return 2;
	}

	static void SetPerspectiveApproach(T* p, lua_State* L, float speed)
	{
		p->m_SpeedfPerspectiveTilt = speed;
		p->m_SpeedfSkew = speed;
	}

	static int Overhead(T* p, lua_State* L)
	{
		const auto original_top = lua_gettop(L);
		lua_pushboolean(L,
						(p->m_fPerspectiveTilt == 0.0f && p->m_fSkew == 0.0f));
		if (lua_toboolean(L, 1) != 0) {
			p->m_fPerspectiveTilt = 0;
			p->m_fSkew = 0;
		}
		if ((lua_isnumber(L, 2) != 0) && original_top >= 2) {
			SetPerspectiveApproach(p, L, FArgGTEZero(L, 2));
		}
		OPTIONAL_RETURN_SELF(original_top);
		return 1;
	}

	static int Incoming(T* p, lua_State* L)
	{
		const auto original_top = lua_gettop(L);
		if ((p->m_fSkew > 0.0f && p->m_fPerspectiveTilt < 0.0f) ||
			(p->m_fSkew < 0.0f && p->m_fPerspectiveTilt > 0.0f)) {
			lua_pushnumber(L, p->m_fSkew);
			lua_pushnumber(L, p->m_SpeedfSkew);
		} else {
			lua_pushnil(L);
			lua_pushnil(L);
		}
		if ((lua_isnumber(L, 1) != 0) && original_top >= 1) {
			const auto value = FArg(1);
			p->m_fPerspectiveTilt = -value;
			p->m_fSkew = value;
		}
		if ((lua_isnumber(L, 2) != 0) && original_top >= 2) {
			SetPerspectiveApproach(p, L, FArgGTEZero(L, 2));
		}
		OPTIONAL_RETURN_SELF(original_top);
		return 2;
	}

	static int Space(T* p, lua_State* L)
	{
		const auto original_top = lua_gettop(L);
		if ((p->m_fSkew > 0.0f && p->m_fPerspectiveTilt > 0.0f) ||
			(p->m_fSkew < 0.0f && p->m_fPerspectiveTilt < 0.0f)) {
			lua_pushnumber(L, p->m_fSkew);
			lua_pushnumber(L, p->m_SpeedfSkew);
		} else {
			lua_pushnil(L);
			lua_pushnil(L);
		}
		if ((lua_isnumber(L, 1) != 0) && original_top >= 1) {
			const auto value = FArg(1);
			p->m_fPerspectiveTilt = value;
			p->m_fSkew = value;
		}
		if ((lua_isnumber(L, 2) != 0) && original_top >= 2) {
			SetPerspectiveApproach(p, L, FArgGTEZero(L, 2));
		}
		OPTIONAL_RETURN_SELF(original_top);
		return 2;
	}

	static int Hallway(T* p, lua_State* L)
	{
		const auto original_top = lua_gettop(L);
		if (p->m_fSkew == 0.0f && p->m_fPerspectiveTilt < 0.0f) {
			lua_pushnumber(L, -p->m_fPerspectiveTilt);
			lua_pushnumber(L, p->m_SpeedfPerspectiveTilt);
		} else {
			lua_pushnil(L);
			lua_pushnil(L);
		}
		if ((lua_isnumber(L, 1) != 0) && original_top >= 1) {
			p->m_fPerspectiveTilt = -FArg(1);
			p->m_fSkew = 0;
		}
		if ((lua_isnumber(L, 2) != 0) && original_top >= 2) {
			SetPerspectiveApproach(p, L, FArgGTEZero(L, 2));
		}
		OPTIONAL_RETURN_SELF(original_top);
		return 2;
	}

	static int Distant(T* p, lua_State* L)
	{
		const auto original_top = lua_gettop(L);
		if (p->m_fSkew == 0.0f && p->m_fPerspectiveTilt > 0.0f) {
			lua_pushnumber(L, p->m_fPerspectiveTilt);
			lua_pushnumber(L, p->m_SpeedfPerspectiveTilt);
		} else {
			lua_pushnil(L);
			lua_pushnil(L);
		}
		if ((lua_isnumber(L, 1) != 0) && original_top >= 1) {
			p->m_fPerspectiveTilt = FArg(1);
			p->m_fSkew = 0;
		}
		if ((lua_isnumber(L, 2) != 0) && original_top >= 2) {
			SetPerspectiveApproach(p, L, FArgGTEZero(L, 2));
		}
		OPTIONAL_RETURN_SELF(original_top);
		return 2;
	}

	DEFINE_METHOD(UsingReverse,
				  m_fScrolls[PlayerOptions::SCROLL_REVERSE] == 1.0f);
	DEFINE_METHOD(UsingPractice, m_bPractice == true);

	static int GetReversePercentForColumn(T* p, lua_State* L)
	{
		const auto colNum = IArg(1);
		const auto numColumns =
		  GAMESTATE->GetCurrentStyle(p->m_pn)->m_iColsPerPlayer;

		// We don't want to go outside the bounds.
		if (colNum < 0 || colNum > numColumns)
			lua_pushnil(L);
		else
			lua_pushnumber(L, p->GetReversePercentForColumn(colNum));

		return 1;
	}

	static int GetStepAttacks(T* p, lua_State* L)
	{
		lua_pushnumber(
		  L, (p->m_fNoAttack > 0 || p->m_fRandAttack > 0 ? false : true));
		return 1;
	}

	static int ContainsTransformOrTurn(T* p, lua_State* L)
	{
		lua_pushboolean(L, p->ContainsTransformOrTurn());
		return 1;
	}

	static int GetInvalidatingMods(T* p, lua_State* L)
	{
		const auto mods = p->GetInvalidatingModifiers();
		LuaHelpers::CreateTableFromArray(mods, L);
		return 1;
	}

	static int FromString(T* p, lua_State* L)
	{
		p->FromString(SArg(1));
		COMMON_RETURN_SELF;
	}

	LunaPlayerOptions()
	{
		ADD_METHOD(IsEasierForSongAndSteps);

		ADD_METHOD(LifeSetting);
		ADD_METHOD(DrainSetting);
		ADD_METHOD(BatteryLives);
		ADD_METHOD(TimeSpacing);
		ADD_METHOD(MaxScrollBPM);
		ADD_METHOD(ScrollSpeed);
		ADD_METHOD(ScrollBPM);

		// verification == need to crosscheck behavior vs sm5.1 and/or nITG

		ADD_METHOD(Boost);
		ADD_METHOD(Brake);
		ADD_METHOD(Wave);
		ADD_METHOD(WavePeriod); // functional but needs verification
		ADD_METHOD(Expand);
		ADD_METHOD(ExpandPeriod);	 // functional but needs verification
		ADD_METHOD(TanExpand);		 // functional but needs verification
		ADD_METHOD(TanExpandPeriod); // functional but needs verification
		ADD_METHOD(Boomerang);

		ADD_METHOD(Drunk);
		ADD_METHOD(Dizzy);
		ADD_METHOD(Confusion);
		ADD_METHOD(ConfusionOffset);  // functional, verified!
		ADD_METHOD(ConfusionX);		  // functional, verified!
		ADD_METHOD(ConfusionXOffset); // functional, verified!
		ADD_METHOD(ConfusionY);		  // functional, verified!
		ADD_METHOD(ConfusionYOffset); // functional, verified!
		ADD_METHOD(Mini);
		ADD_METHOD(Tiny);
		ADD_METHOD(Flip);
		ADD_METHOD(Invert);
		ADD_METHOD(Tornado);
		ADD_METHOD(Tipsy);
		ADD_METHOD(Bumpy);
		ADD_METHOD(Beat);
		ADD_METHOD(Xmode); // depends on player2, so broken, fix later mb
		ADD_METHOD(Twirl);
		ADD_METHOD(Roll);
		ADD_METHOD(AttenuateX);		   // functional, verified!
		ADD_METHOD(AttenuateY);		   // functional, verified!
		ADD_METHOD(AttenuateZ);		   // functional, verified!
		ADD_METHOD(Bounce);			   // functional but needs verification
		ADD_METHOD(BouncePeriod);	   // functional but needs verification
		ADD_METHOD(BounceOffset);	   // functional but needs verification
		ADD_METHOD(BounceZ);		   // functional but needs verification
		ADD_METHOD(BounceZPeriod);	   // functional but needs verification
		ADD_METHOD(BounceZOffset);	   // functional but needs verification
		ADD_METHOD(Digital);		   // functional but needs verification
		ADD_METHOD(DigitalSteps);	   // functional but needs verification
		ADD_METHOD(DigitalPeriod);	   // functional but needs verification
		ADD_METHOD(DigitalOffset);	   // functional but needs verification
		ADD_METHOD(TanDigital);		   // functional but needs verification
		ADD_METHOD(TanDigitalSteps);   // functional but needs verification
		ADD_METHOD(TanDigitalPeriod);  // functional but needs verification
		ADD_METHOD(TanDigitalOffset);  // functional but needs verification
		ADD_METHOD(DigitalZ);		   // functional but needs verification
		ADD_METHOD(DigitalZSteps);	   // functional but needs verification
		ADD_METHOD(DigitalZPeriod);	   // functional but needs verification
		ADD_METHOD(DigitalZOffset);	   // functional but needs verification
		ADD_METHOD(TanDigitalZ);	   // functional but needs verification
		ADD_METHOD(TanDigitalZSteps);  // functional but needs verification
		ADD_METHOD(TanDigitalZPeriod); // functional but needs verification
		ADD_METHOD(TanDigitalZOffset); // functional but needs verification
		ADD_METHOD(Zigzag);			   // functional but needs verification
		ADD_METHOD(ZigzagPeriod);	   // functional but needs verification
		ADD_METHOD(ZigzagOffset);	   // functional but needs verification
		ADD_METHOD(ZigzagZ);		   // functional but needs verification
		ADD_METHOD(ZigzagZPeriod);	   // functional but needs verification
		ADD_METHOD(ZigzagZOffset);	   // functional but needs verification

		ADD_METHOD(Hidden);
		ADD_METHOD(HiddenOffset);
		ADD_METHOD(Sudden);
		ADD_METHOD(SuddenOffset);
		ADD_METHOD(Stealth);
		ADD_METHOD(StealthType);		  // not functional? fix it?
		ADD_METHOD(StealthPastReceptors); // not functional. fix it
		ADD_METHOD(Blink);
		ADD_METHOD(RandomVanish);

		ADD_METHOD(Reverse);
		ADD_METHOD(Split);
		ADD_METHOD(Alternate);
		ADD_METHOD(Cross);
		ADD_METHOD(Centered);

		ADD_METHOD(Dark);
		ADD_METHOD(Blind);
		ADD_METHOD(Cover);

		ADD_METHOD(Cosecant); // functional but needs verification

		ADD_METHOD(RandAttack);
		ADD_METHOD(NoAttack);

		ADD_METHOD(TurnNone);
		ADD_METHOD(Mirror);
		ADD_METHOD(Backwards);
		ADD_METHOD(Left);
		ADD_METHOD(Right);
		ADD_METHOD(Shuffle);
		ADD_METHOD(SoftShuffle);
		ADD_METHOD(SuperShuffle);
		ADD_METHOD(NoHolds);
		ADD_METHOD(NoRolls);
		ADD_METHOD(NoMines);
		ADD_METHOD(Little);
		ADD_METHOD(Wide);
		ADD_METHOD(Big);
		ADD_METHOD(Quick);
		ADD_METHOD(BMRize);
		ADD_METHOD(Skippy);
		ADD_METHOD(Mines);
		ADD_METHOD(AttackMines);
		ADD_METHOD(Echo);
		ADD_METHOD(Stomp);
		ADD_METHOD(JackJS);
		ADD_METHOD(AnchorJS);
		ADD_METHOD(IcyWorld);
		ADD_METHOD(Planted);
		ADD_METHOD(Floored);
		ADD_METHOD(Twister);
		ADD_METHOD(HoldRolls);
		ADD_METHOD(NoJumps);
		ADD_METHOD(NoHands);
		ADD_METHOD(NoLifts);
		ADD_METHOD(NoFakes);
		ADD_METHOD(NoQuads);
		ADD_METHOD(NoStretch);

		ADD_METHOD(CMod);
		ADD_METHOD(XMod);
		ADD_METHOD(MMod);

		ADD_METHOD(Overhead);
		ADD_METHOD(Incoming);
		ADD_METHOD(Space);
		ADD_METHOD(Hallway);
		ADD_METHOD(Distant);

		ADD_METHOD(PlayerAutoPlay);
		ADD_METHOD(Tilt);
		ADD_METHOD(Skew);
		ADD_METHOD(Passmark);
		ADD_METHOD(RandomSpeed);

		ADD_MULTICOL_METHOD(MoveX); // functional, verified!
		ADD_MULTICOL_METHOD(MoveY); // functional, verified!
		ADD_MULTICOL_METHOD(MoveZ); // functional, verified!
		ADD_MULTICOL_METHOD(Bumpy);
		ADD_MULTICOL_METHOD(Tiny);
		ADD_MULTICOL_METHOD(ConfusionOffset);  // functional, verified!
		ADD_MULTICOL_METHOD(ConfusionXOffset); // functional and verified
											   // but doesn't auto reset
											   // to 0? investigate
		ADD_MULTICOL_METHOD(ConfusionYOffset); // functional, verified!
		ADD_MULTICOL_METHOD(Stealth);		   // functional, verified!
		ADD_MULTICOL_METHOD(Reverse);		   // functional, verified!

		ADD_METHOD(NoteSkin);
		ADD_METHOD(MuteOnError);
		ADD_METHOD(FailSetting);
		ADD_METHOD(MinTNSToHideNotes);

		ADD_METHOD(UsingReverse);
		ADD_METHOD(GetReversePercentForColumn);

		ADD_METHOD(GetStepAttacks);
		ADD_METHOD(ContainsTransformOrTurn);
		ADD_METHOD(GetInvalidatingMods);
		// ADD_METHOD(PracticeMode); -- Restrict theme access to practice mode
		ADD_METHOD(UsingPractice);

		ADD_METHOD(FromString);
	}
};

LUA_REGISTER_CLASS(PlayerOptions)
// lua end
