#ifndef PLAYER_OPTIONS_H
#define PLAYER_OPTIONS_H

class Song;
class Steps;
struct lua_State;

#define ONE(arr)                                                               \
	{                                                                          \
		for (unsigned Z = 0; Z < ARRAYLEN(arr); ++Z)                           \
			(arr)[Z] = 1.0f;                                                   \
	}

#include "GameConstantsAndTypes.h"
#include "PlayerNumber.h"
#include "Etterna/Singletons/PrefsManager.h"

enum LifeType
{
	LifeType_Bar,
	LifeType_Battery,
	LifeType_Time,
	NUM_LifeType,
	LifeType_Invalid
};
auto
LifeTypeToString(LifeType cat) -> const std::string&;
auto
LifeTypeToLocalizedString(LifeType cat) -> const std::string&;
LuaDeclareType(LifeType);

enum DrainType
{
	DrainType_Normal,
	DrainType_NoRecover,
	DrainType_SuddenDeath,
	NUM_DrainType,
	DrainType_Invalid
};
auto
DrainTypeToString(DrainType cat) -> const std::string&;
auto
DrainTypeToLocalizedString(DrainType cat) -> const std::string&;
LuaDeclareType(DrainType);

/** @brief Per-player options that are not saved between sessions. */
class PlayerOptions
{
  public:
	/**
	 * @brief Set up the PlayerOptions with some reasonable defaults.
	 *
	 * This code was taken from Init() to use proper initialization. */
	PlayerOptions()
	  : m_MinTNSToHideNotes(PREFSMAN->m_MinTNSToHideNotes)
	{
		m_bCosecant = false;
		m_bStealthPastReceptors = false;
		m_bStealthType = false;
		m_sNoteSkin = "";
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
		ZERO(m_fConfusionX);
		ONE(m_SpeedfConfusionX);
		ZERO(m_fConfusionY);
		ONE(m_SpeedfConfusionY);
		ZERO(m_fConfusionZ);
		ONE(m_SpeedfConfusionZ);
		ZERO(m_fStealth);
		ONE(m_SpeedfStealth);
		ZERO(m_fReverse);
		ONE(m_SpeedfReverse);
	};
	void Init();
	void Approach(const PlayerOptions& other, float fDeltaSeconds);
	[[nodiscard]] auto GetString(bool bForceNoteSkin = false) const
	  -> std::string;
	[[nodiscard]] auto GetSavedPrefsString() const
	  -> std::string; // only the basic options that players would want for
					  // every song
	enum ResetPrefsType
	{
		saved_prefs,
	};
	void ResetPrefs(ResetPrefsType type);
	void ResetSavedPrefs() { ResetPrefs(saved_prefs); };
	void GetMods(std::vector<std::string>& AddTo,
				 bool bForceNoteSkin = false) const;
	void GetTurnMods(std::vector<std::string>& AddTo);
	void ResetModsToStringVector(std::vector<std::string> mods);
	void ResetToggleableMods();
	void GetLocalizedMods(std::vector<std::string>& AddTo) const;
	void FromString(const std::string& sMultipleMods);
	auto FromOneModString(const std::string& sOneMod,
						  std::string& sErrorDetailOut)
	  -> bool; // On error, return
			   // false and optionally
			   // set sErrorDetailOut
	void ChooseRandomModifiers();
	// Returns true for modifiers that should invalidate a score or otherwise
	// make it impossible to calculate Replay info
	[[nodiscard]] auto ContainsTransformOrTurn() const -> bool;

	[[nodiscard]] auto GetInvalidatingModifiers() const
	  -> std::vector<std::string>;

	// Lua
	void PushSelf(lua_State* L);

	auto operator==(const PlayerOptions& other) const -> bool;
	auto operator!=(const PlayerOptions& other) const -> bool
	{
		return !operator==(other);
	}
	auto operator=(PlayerOptions const& other) -> PlayerOptions&;

	/** @brief The various acceleration mods. */
	enum Accel
	{
		ACCEL_BOOST, /**< The arrows start slow, then zoom towards the targets.
					  */
		ACCEL_BRAKE, /**< The arrows start fast, then slow down as they approach
						the targets. */
		ACCEL_WAVE,
		ACCEL_WAVE_PERIOD,
		ACCEL_EXPAND,
		ACCEL_EXPAND_PERIOD,
		ACCEL_TAN_EXPAND,
		ACCEL_TAN_EXPAND_PERIOD,
		ACCEL_BOOMERANG, /**< The arrows start from above the targets, go down,
							then come back up. */
		NUM_ACCELS
	};
	enum Effect
	{
		EFFECT_DRUNK,
		EFFECT_DIZZY,
		EFFECT_CONFUSION,
		EFFECT_CONFUSION_OFFSET,
		EFFECT_CONFUSION_X,
		EFFECT_CONFUSION_X_OFFSET,
		EFFECT_CONFUSION_Y,
		EFFECT_CONFUSION_Y_OFFSET,
		EFFECT_MINI,
		EFFECT_TINY,
		EFFECT_FLIP,
		EFFECT_INVERT,
		EFFECT_TORNADO,
		EFFECT_TIPSY,
		EFFECT_BUMPY,
		EFFECT_BEAT,
		EFFECT_XMODE,
		EFFECT_TWIRL,
		EFFECT_ROLL,
		EFFECT_ATTENUATE_X,
		EFFECT_ATTENUATE_Y,
		EFFECT_ATTENUATE_Z,
		EFFECT_BOUNCE,
		EFFECT_BOUNCE_PERIOD,
		EFFECT_BOUNCE_OFFSET,
		EFFECT_BOUNCE_Z,
		EFFECT_BOUNCE_Z_PERIOD,
		EFFECT_BOUNCE_Z_OFFSET,
		EFFECT_DIGITAL,
		EFFECT_DIGITAL_STEPS,
		EFFECT_DIGITAL_PERIOD,
		EFFECT_DIGITAL_OFFSET,
		EFFECT_TAN_DIGITAL,
		EFFECT_TAN_DIGITAL_STEPS,
		EFFECT_TAN_DIGITAL_PERIOD,
		EFFECT_TAN_DIGITAL_OFFSET,
		EFFECT_DIGITAL_Z,
		EFFECT_DIGITAL_Z_STEPS,
		EFFECT_DIGITAL_Z_PERIOD,
		EFFECT_DIGITAL_Z_OFFSET,
		EFFECT_TAN_DIGITAL_Z,
		EFFECT_TAN_DIGITAL_Z_STEPS,
		EFFECT_TAN_DIGITAL_Z_PERIOD,
		EFFECT_TAN_DIGITAL_Z_OFFSET,
		NUM_EFFECTS
	};
	/** @brief The various appearance mods. */
	enum Appearance
	{
		APPEARANCE_HIDDEN,		  /**< The arrows disappear partway up. */
		APPEARANCE_HIDDEN_OFFSET, /**< This determines when the arrows
									 disappear. */
		APPEARANCE_SUDDEN,		  /**< The arrows appear partway up. */
		APPEARANCE_SUDDEN_OFFSET, /**< This determines when the arrows appear.
								   */
		APPEARANCE_STEALTH,		  /**< The arrows are not shown at all. */
		APPEARANCE_BLINK,		  /**< The arrows blink constantly. */
		APPEARANCE_RANDOMVANISH, /**< The arrows disappear, and then reappear in
									a different column. */
		NUM_APPEARANCES
	};
	/** @brief The various turn mods. */
	enum Turn
	{
		TURN_NONE = 0, /**< No turning of the arrows is performed. */
		TURN_MIRROR, /**< The arrows are mirrored from their normal position. */
		TURN_BACKWARDS, /**< The arrows are turned 180 degrees. This does NOT
						   always equal mirror. */
		TURN_LEFT,		/**< The arrows are turned 90 degrees to the left. */
		TURN_RIGHT,		/**< The arrows are turned 90 degress to the right. */
		TURN_SHUFFLE, /**< Some of the arrow columns are changed throughout the
						 whole song. */
		TURN_SOFT_SHUFFLE,	/**< Only shuffle arrow columns on an axis of
							   symmetry. */
		TURN_SUPER_SHUFFLE, /**< Every arrow is placed on a random column. */
		NUM_TURNS
	};
	enum Transform
	{
		TRANSFORM_NOHOLDS,
		TRANSFORM_NOROLLS,
		TRANSFORM_NOMINES,
		TRANSFORM_LITTLE,
		TRANSFORM_WIDE,
		TRANSFORM_BIG,
		TRANSFORM_QUICK,
		TRANSFORM_BMRIZE,
		TRANSFORM_SKIPPY,
		TRANSFORM_MINES,
		TRANSFORM_ATTACKMINES,
		TRANSFORM_ECHO,
		TRANSFORM_STOMP,
		TRANSFORM_JACKJS,
		TRANSFORM_ANCHORJS,
		TRANSFORM_ICYWORLD,
		TRANSFORM_PLANTED,
		TRANSFORM_FLOORED,
		TRANSFORM_TWISTER,
		TRANSFORM_HOLDROLLS,
		TRANSFORM_NOJUMPS,
		TRANSFORM_NOHANDS,
		TRANSFORM_NOLIFTS,
		TRANSFORM_NOFAKES,
		TRANSFORM_NOQUADS,
		TRANSFORM_NOSTRETCH,
		NUM_TRANSFORMS
	};
	enum Scroll
	{
		SCROLL_REVERSE = 0,
		SCROLL_SPLIT,
		SCROLL_ALTERNATE,
		SCROLL_CROSS,
		SCROLL_CENTERED,
		NUM_SCROLLS
	};

	[[nodiscard]] auto GetReversePercentForColumn(int iCol) const
	  -> float; // accounts for all Directions

	PlayerNumber m_pn{ PLAYER_1 }; // Needed for fetching the style.

	LifeType m_LifeType{ LifeType_Bar };
	DrainType m_DrainType{ DrainType_Normal }; // only used with LifeBar
	int m_BatteryLives{ 4 };
	/* All floats have a corresponding speed setting, which determines how fast
	 * PlayerOptions::Approach approaches. */
	bool m_bSetScrollSpeed{
		false
	}; // true if the scroll speed was set by FromString
	float m_fTimeSpacing{ 0 },
	  m_SpeedfTimeSpacing{ 1.0F }; // instead of Beat spacing (CMods, mMods)
	float m_fMaxScrollBPM{ 0 }, m_SpeedfMaxScrollBPM{ 1.0F };
	float m_fScrollSpeed{ 1.0F },
	  m_SpeedfScrollSpeed{ 1.0F }; // used if !m_bTimeSpacing (xMods)
	float m_fScrollBPM{ 200 },
	  m_SpeedfScrollBPM{ 1.0F }; // used if m_bTimeSpacing (CMod)

	float m_fAccels[NUM_ACCELS]{}, m_SpeedfAccels[NUM_ACCELS]{};
	float m_fEffects[NUM_EFFECTS]{}, m_SpeedfEffects[NUM_EFFECTS]{};
	float m_fAppearances[NUM_APPEARANCES]{},
	  m_SpeedfAppearances[NUM_APPEARANCES]{};
	float m_fScrolls[NUM_SCROLLS]{}, m_SpeedfScrolls[NUM_SCROLLS]{};

	float m_fDark{ 0 }, m_SpeedfDark{ 1.0F };
	float m_fBlind{ 0 }, m_SpeedfBlind{ 1.0F };
	float m_fCover{ 0 }, m_SpeedfCover{
		1.0F
	}; // hide the background per-player--can't think of a good name

	float m_fRandAttack{ 0 }, m_SpeedfRandAttack{ 1.0F };
	float m_fNoAttack{ 0 }, m_SpeedfNoAttack{ 1.0F };
	float m_fPlayerAutoPlay{ 0 }, m_SpeedfPlayerAutoPlay{ 1.0F };
	float m_fPerspectiveTilt{ 0 },
	  m_SpeedfPerspectiveTilt{ 1.0F }; // -1 = near, 0 = overhead, +1 = space
	float m_fSkew{ 0 }, m_SpeedfSkew{ 1.0F }; // 0 = vanish point is in center
											  // of player, 1 = vanish point is
											  // in center of screen
	/* If this is > 0, then the player must have life above this value at the
	 * end of the song to pass.  This is independent of SongOptions::m_FailType.
	 */
	float m_fPassmark{ 0 }, m_SpeedfPassmark{ 1.0F };
	float m_fRandomSpeed{ 0 }, m_SpeedfRandomSpeed{ 1.0F };
	bool m_bTurns[NUM_TURNS]{};
	bool m_bTransforms[NUM_TRANSFORMS]{};
	bool m_bMuteOnError{ false };
	bool m_bPractice{ false };

	/* The maximum column number is 16.*/
	float m_fMovesX[16], m_SpeedfMovesX[16];
	float m_fMovesY[16], m_SpeedfMovesY[16];
	float m_fMovesZ[16], m_SpeedfMovesZ[16];
	float m_fBumpy[16], m_SpeedfBumpy[16];
	float m_fTiny[16], m_SpeedfTiny[16];
	float m_fConfusionX[16], m_SpeedfConfusionX[16];
	float m_fConfusionY[16], m_SpeedfConfusionY[16];
	float m_fConfusionZ[16], m_SpeedfConfusionZ[16];
	float m_fStealth[16], m_SpeedfStealth[16];
	float m_fReverse[16], m_SpeedfReverse[16];

	bool m_bCosecant{ false };
	bool m_bStealthType{ false };
	bool m_bStealthPastReceptors{ false };
	/** @brief The method for which a player can fail a song. */
	FailType m_FailType{ FailType_Immediate };
	TapNoteScore m_MinTNSToHideNotes;

	/**
	 * @brief The Noteskin to use.
	 *
	 * If an empty string, it means to not change from the default. */
	std::string m_sNoteSkin;

	void NextAccel();
	void NextEffect();
	void NextAppearance();
	void NextTurn();
	void NextTransform();
	void NextPerspective();
	void NextScroll();

	auto GetFirstAccel() -> Accel;
	auto GetFirstEffect() -> Effect;
	auto GetFirstAppearance() -> Appearance;
	auto GetFirstScroll() -> Scroll;

	void SetOneAccel(Accel a);
	void SetOneEffect(Effect e);
	void SetOneAppearance(Appearance a);
	void SetOneScroll(Scroll s);
	void ToggleOneTurn(Turn t);

	// return true if any mods being used will make the song(s) easier
	auto IsEasierForSongAndSteps(Song* pSong,
								 Steps* pSteps,
								 PlayerNumber pn) const -> bool;
};
#define ADD_MULTICOL_METHOD(method_name)                                       \
	ADD_METHOD(method_name##1);                                                \
	ADD_METHOD(method_name##2);                                                \
	ADD_METHOD(method_name##3);                                                \
	ADD_METHOD(method_name##4);                                                \
	ADD_METHOD(method_name##5);                                                \
	ADD_METHOD(method_name##6);                                                \
	ADD_METHOD(method_name##7);                                                \
	ADD_METHOD(method_name##8);                                                \
	ADD_METHOD(method_name##9);                                                \
	ADD_METHOD(method_name##10);                                               \
	ADD_METHOD(method_name##11);                                               \
	ADD_METHOD(method_name##12);                                               \
	ADD_METHOD(method_name##13);                                               \
	ADD_METHOD(method_name##14);                                               \
	ADD_METHOD(method_name##15);                                               \
	ADD_METHOD(method_name##16);
#define MULTICOL_FLOAT_INTERFACE(func_name, member, valid)                     \
	FLOAT_INTERFACE(func_name##1, member[0], valid);                           \
	FLOAT_INTERFACE(func_name##2, member[1], valid);                           \
	FLOAT_INTERFACE(func_name##3, member[2], valid);                           \
	FLOAT_INTERFACE(func_name##4, member[3], valid);                           \
	FLOAT_INTERFACE(func_name##5, member[4], valid);                           \
	FLOAT_INTERFACE(func_name##6, member[5], valid);                           \
	FLOAT_INTERFACE(func_name##7, member[6], valid);                           \
	FLOAT_INTERFACE(func_name##8, member[7], valid);                           \
	FLOAT_INTERFACE(func_name##9, member[8], valid);                           \
	FLOAT_INTERFACE(func_name##10, member[9], valid);                          \
	FLOAT_INTERFACE(func_name##11, member[10], valid);                         \
	FLOAT_INTERFACE(func_name##12, member[11], valid);                         \
	FLOAT_INTERFACE(func_name##13, member[12], valid);                         \
	FLOAT_INTERFACE(func_name##14, member[13], valid);                         \
	FLOAT_INTERFACE(func_name##15, member[14], valid);                         \
	FLOAT_INTERFACE(func_name##16, member[15], valid);

#endif
