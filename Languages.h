/*
RailControl - Model Railway Control Software

Copyright (c) 2017-2020 Dominik (Teddy) Mahrer - www.railcontrol.org

RailControl is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3, or (at your option) any
later version.

RailControl is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RailControl; see the file LICENCE. If not see
<http://www.gnu.org/licenses/>.
*/

#pragma once

#include <map>
#include <string>

#include "DataModel/AccessoryBase.h"
#include "DataTypes.h"

class Languages
{
	public:
		enum TextSelector : unsigned int
		{
			Text180Deg,
			Text90DegAntiClockwise,
			Text90DegClockwise,
			TextAccessories,
			TextAccessory,
			TextAccessoryAddressDccTooHigh,
			TextAccessoryAddressMmTooHigh,
			TextAccessoryDeleted,
			TextAccessoryDoesNotExist,
			TextAccessoryIsLocked,
			TextAccessorySaved,
			TextAccessorySenderThreadStarted,
			TextAccessoryStateIsGreen,
			TextAccessoryStateIsRed,
			TextAccessoryUpdated,
			TextActualAndStoredProtocolsDiffer,
			TextAddAccessory,
			TextAddFeedback,
			TextAddRoute,
			TextAddSignal,
			TextAddSwitch,
			TextAddTrack,
			TextAddingFeedback,
			TextAddress,
			TextAddressMustBeHigherThen0,
			TextAllTrains,
			TextAllowedTrains,
			TextAreYouSureToDelete,
			TextAtLock,
			TextAtUnlock,
			TextAutomaticallyAddUnknownFeedbacks,
			TextAutomode,
			TextBasic,
			TextBlockTrack,
			TextBoosterIsTurnedOff,
			TextBoosterIsTurnedOn,
			TextBridge,
			TextBufferStop,
			TextCV,
			TextCanNotOpenLibrary,
			TextCanNotStartAlreadyRunning,
			TextCanNotStartInErrorState,
			TextCanNotStartNotOnTrack,
			TextCheckSumError,
			TextClosingSQLite,
			TextConfigFileReceivedWithSize,
			TextConnectionFailed,
			TextConnectionRefused,
			TextConnectionReset,
			TextControl,
			TextControlDeleted,
			TextControlDoesNotAnswer,
			TextControlDoesNotExist,
			TextControlReturnedBadParameter,
			TextControlReturnedError,
			TextControlReturnedOnHalt,
			TextControlReturnedPowerOff,
			TextControlReturnedQueueFull,
			TextControlReturnedQueueNearlyFull,
			TextControlReturnedUnknownErrorCode,
			TextControlSaved,
			TextControls,
			TextCopyingFromTo,
			TextCreatingTable,
			TextCreepAt,
			TextCreepingSpeed,
			TextCs2MasterFound,
			TextCs2MasterLocoAddressProtocol,
			TextCs2MasterLocoFunctionIconType,
			TextCs2MasterLocoFunctionIconTypeTimer,
			TextCs2MasterLocoName,
			TextCs2MasterLocoOldName,
			TextCs2MasterLocoRemove,
			TextCs2MinorVersionIsNot4,
			TextDcc,
			TextDebounceThreadStarted,
			TextDebounceThreadTerminated,
			TextDebouncer,
			TextDebug,
			TextDefaultSwitchingDuration,
			TextDelete,
			TextDeleteAccessory,
			TextDeleteControl,
			TextDeleteFeedback,
			TextDeleteLayer,
			TextDeleteLoco,
			TextDeleteRoute,
			TextDeleteSignal,
			TextDeleteSwitch,
			TextDeleteTrack,
			TextDestinationSignalTrack,
			TextDeviceOnCanBus,
			TextDifferentOrientations,
			TextDifferentPushpullTypes,
			TextDirect,
			TextDoNotCare,
			TextDroppingTable,
			TextDuration,
			TextEdit,
			TextEditAccessories,
			TextEditAccessory,
			TextEditControls,
			TextEditFeedback,
			TextEditFeedbacks,
			TextEditLayers,
			TextEditLocos,
			TextEditRoute,
			TextEditRoutes,
			TextEditSettings,
			TextEditSignal,
			TextEditSignals,
			TextEditSwitch,
			TextEditSwitches,
			TextEditTrack,
			TextEditTracks,
			TextEnglish,
			TextError,
			TextExecutingRoute,
			TextExitRailControl,
			TextFeedback,
			TextFeedbackChange,
			TextFeedbackDeleted,
			TextFeedbackDoesNotExist,
			TextFeedbackSaved,
			TextFeedbackStateIsOff,
			TextFeedbackStateIsOn,
			TextFeedbackUpdated,
			TextFeedbacks,
			TextFoundAccessoryInEcosDatabase,
			TextFoundFeedbackModuleInEcosDatabase,
			TextFoundLocoInEcosDatabase,
			TextGerman,
			TextGreen,
			TextHasAlreadyReservedRoute,
			TextHasNotReachedDestination,
			TextHeadingToVia,
			TextHeadingToViaVia,
			TextHeartBeatThreadStarted,
			TextHeightIs0,
			TextHint,
			TextHintCcSchnitte,
			TextHintCs2Tcp,
			TextHintCs2Udp,
			TextHintEcos,
			TextHintHsi88,
			TextHintM6051,
			TextHintOpenDcc,
			TextHintRM485,
			TextHintVirtual,
			TextHintZ21,
			TextHitOverrun,
			TextHsi88Configured,
			TextHsi88ErrorConfiguring,
			TextHttpConnectionClose,
			TextHttpConnectionNotFound,
			TextHttpConnectionNotImplemented,
			TextHttpConnectionOpen,
			TextHttpConnectionRequest,
			TextIPAddress,
			TextIndex,
			TextInfo,
			TextInvalidControlID,
			TextInvalidDataReceived,
			TextInverted,
			TextIsInAutomodeWithoutRouteTrack,
			TextIsInErrorState,
			TextIsInInvalidAutomodeState,
			TextIsInManualState,
			TextIsInTerminatedState,
			TextIsLockedBy,
			TextIsNotFree,
			TextIsNotOnTrack,
			TextIsNowInAutoMode,
			TextIsNowInManualMode,
			TextIsOnOcupiedTrack,
			TextIsRunningWaitingUntilDestination,
			TextIsUpToDate,
			TextLanguage,
			TextLayer1,
			TextLayer1IsUndeletable,
			TextLayerDeleted,
			TextLayerDoesNotExist,
			TextLayerSaved,
			TextLayerUpdated,
			TextLayers,
			TextLeft,
			TextLength,
			TextLibraryLoaded,
			TextLibraryUnloaded,
			TextLink,
			TextLoadedAccessory,
			TextLoadedControl,
			TextLoadedFeedback,
			TextLoadedLayer,
			TextLoadedLoco,
			TextLoadedRoute,
			TextLoadedSignal,
			TextLoadedSwitch,
			TextLoadedTrack,
			TextLoco,
			TextLocoAddressDccTooHigh,
			TextLocoAddressMmTooHigh,
			TextLocoDeleted,
			TextLocoDirectionOfTravelIsLeft,
			TextLocoDirectionOfTravelIsRight,
			TextLocoDoesNotExist,
			TextLocoEventDetected,
			TextLocoFunctionIsOff,
			TextLocoFunctionIsOn,
			TextLocoHasReachedDestination,
			TextLocoIsInAutoMode,
			TextLocoIsInManualMode,
			TextLocoIsOnTrack,
			TextLocoIsReleased,
			TextLocoSaved,
			TextLocoSpeedIs,
			TextLocoUpdated,
			TextLocos,
			TextLogLevel,
			TextLongestUnused,
			TextLookingForDestination,
			TextMaerklinMotorola,
			TextManager,
			TextMaxSpeed,
			TextMaxTrainLength,
			TextMembers,
			TextMinTrackLength,
			TextMinTrainLength,
			TextMultipleUnit,
			TextMyUidHash,
			TextName,
			TextNetworkUnreachable,
			TextNew,
			TextNoAnswerFromDecoder,
			TextNoControlSupportsProgramming,
			TextNoPushPull,
			TextNoRotation,
			TextNoS88Modules,
			TextNoValidRouteFound,
			TextNotImplemented,
			TextNrOfFunctions,
			TextNrOfS88Modules,
			TextNrOfS88ModulesConfigured,
			TextNrOfS88ModulesOnBus,
			TextNrOfTracksToReserve,
			TextOff,
			TextOn,
			TextOpeningSQLite,
			TextOrientation,
			TextOverrunAt,
			TextParameterFoundInConfigFile,
			TextPin,
			TextPosX,
			TextPosY,
			TextPosZ,
			TextPosition,
			TextPositionAlreadyInUse,
			TextProgramDccPomAccessoryRead,
			TextProgramDccPomAccessoryWrite,
			TextProgramDccPomLocoRead,
			TextProgramDccPomLocoWrite,
			TextProgramDccRead,
			TextProgramDccWrite,
			TextProgramMfxRead,
			TextProgramMfxWrite,
			TextProgramMm,
			TextProgramMmPom,
			TextProgramMode,
			TextProgramModeDccDirect,
			TextProgramModeDccPomAccessory,
			TextProgramModeDccPomLoco,
			TextProgramModeMfx,
			TextProgramModeMm,
			TextProgramModeMmPom,
			TextProgramReadValue,
			TextProgrammer,
			TextProgrammingMode,
			TextProtocol,
			TextProtocolNotSupported,
			TextPushPullOnly,
			TextPushPullTrain,
			TextQuery,
			TextRM485ModuleFound,
			TextRailControlStarted,
			TextRandom,
			TextReachedItsDestination,
			TextRead,
			TextReadingConfigFile,
			TextReceivedAccessoryCommand,
			TextReceivedDirectionCommand,
			TextReceivedFunctionCommand,
			TextReceivedSignalKill,
			TextReceivedSpeedCommand,
			TextReceiverThreadStarted,
			TextRed,
			TextReducedSpeed,
			TextReducedSpeedAt,
			TextRegister,
			TextRelationTargetNotFound,
			TextRelease,
			TextReleaseAccessory,
			TextReleaseLoco,
			TextReleaseRoute,
			TextReleaseSignal,
			TextReleaseSwitch,
			TextReleaseTrack,
			TextReleaseTrackAndLoco,
			TextReleaseWhenFree,
			TextRemoveBackupFile,
			TextRenamingFromTo,
			TextRestarting,
			TextRight,
			TextRotation,
			TextRoute,
			TextRouteDeleted,
			TextRouteDoesNotExist,
			TextRouteIsLocked,
			TextRouteIsReleased,
			TextRouteSaved,
			TextRouteUpdated,
			TextRoutes,
			TextSQLiteErrorQuery,
			TextSaving,
			TextSelectLocoForTrack,
			TextSelectRouteBy,
			TextSenderSocketCreated,
			TextSerialNumberIs,
			TextSerialPort,
			TextSetAllLocosToAutomode,
			TextSetAllLocosToManualMode,
			TextSetLoco,
			TextSettingAccessory,
			TextSettingAccessoryWithProtocol,
			TextSettingDirectionOfTravel,
			TextSettingDirectionOfTravelWithProtocol,
			TextSettingFunction,
			TextSettingFunctionWithProtocol,
			TextSettingFunctions17_28,
			TextSettingFunctions1_8,
			TextSettingFunctions9_16,
			TextSettingSpeed,
			TextSettingSpeedOrientationLight,
			TextSettingSpeedWithProtocol,
			TextSettings,
			TextSettingsSaved,
			TextShortCircuit,
			TextSignal,
			TextSignalDeleted,
			TextSignalDoesNotExist,
			TextSignalIsLocked,
			TextSignalSaved,
			TextSignalStateIsGreen,
			TextSignalStateIsRed,
			TextSignalUpdated,
			TextSignals,
			TextSimpleLeft,
			TextSimpleRight,
			TextSpanish,
			TextSpeed,
			TextStartLoco,
			TextStartSignalTrack,
			TextStarting,
			TextStopAllLocos,
			TextStopAt,
			TextStopLoco,
			TextStoppingRailControl,
			TextStoppingRequestedBySignal,
			TextStoppingRequestedByWebClient,
			TextStraight,
			TextSwitch,
			TextSwitchDeleted,
			TextSwitchDoesNotExist,
			TextSwitchEventDetected,
			TextSwitchIsLocked,
			TextSwitchSaved,
			TextSwitchStateIsStraight,
			TextSwitchStateIsTurnout,
			TextSwitchUpdated,
			TextSwitches,
			TextSystemDefault,
			TextTerminatingAccessorySenderThread,
			TextTerminatingHeartBeatThread,
			TextTerminatingReceiverThread,
			TextTerminatingSenderSocket,
			TextTimestampAlreadySet,
			TextTimestampNotSet,
			TextTimestampSet,
			TextTooManyS88Modules,
			TextTrack,
			TextTrackDeleted,
			TextTrackDoesNotExist,
			TextTrackIsInUse,
			TextTrackSaved,
			TextTrackStatusIsBlocked,
			TextTrackStatusIsBlockedAndOccupied,
			TextTrackStatusIsBlockedAndReserved,
			TextTrackStatusIsFree,
			TextTrackStatusIsOccupied,
			TextTrackStatusIsReserved,
			TextTrackUpdated,
			TextTracks,
			TextTrainIsToLong,
			TextTrainIsToShort,
			TextTrainLength,
			TextTravelSpeed,
			TextTunnelOneSide,
			TextTunnelTwoSides,
			TextTurn,
			TextTurnDirectionOfTravelToLeft,
			TextTurnDirectionOfTravelToRight,
			TextTurningBoosterOff,
			TextTurningBoosterOn,
			TextTurningBoosterOnOrOff,
			TextTurnout,
			TextType,
			TextUnabelToStoreLibraryAddress,
			TextUnableToAddAccessory,
			TextUnableToAddControl,
			TextUnableToAddFeedback,
			TextUnableToAddLayer,
			TextUnableToAddLayer1,
			TextUnableToAddLoco,
			TextUnableToAddLocoToTrack,
			TextUnableToAddRoute,
			TextUnableToAddSignal,
			TextUnableToAddSwitch,
			TextUnableToAddTrack,
			TextUnableToBindSocketToPort,
			TextUnableToBindUdpSocket,
			TextUnableToCalculatePosition,
			TextUnableToCreateStorageHandler,
			TextUnableToCreateTcpSocket,
			TextUnableToCreateUdpSocket,
			TextUnableToCreateUdpSocketForReceivingData,
			TextUnableToCreateUdpSocketForSendingData,
			TextUnableToFindSymbol,
			TextUnableToLock,
			TextUnableToOpenFile,
			TextUnableToOpenSQLite,
			TextUnableToOpenSerial,
			TextUnableToReceiveData,
			TextUnableToReserve,
			TextUnableToResolveAddress,
			TextUnableToSendDataToControl,
			TextUnblockTrack,
			TextUnknownObjectType,
			TextUnloadingControl,
			TextValue,
			TextVersion,
			TextVisible,
			TextWaitAfterRelease,
			TextWaitingTimeBetweenMembers,
			TextWaitingUntilHasStopped,
			TextWarning,
			TextWebServerStarted,
			TextWebServerStopped,
			TextWidthIs0,
			TextWrite,
			TextZ21Black2012,
			TextZ21Black2013,
			TextZ21DoesNotUnderstand,
			TextZ21NotRestricted,
			TextZ21RestrictionsUnknown,
			TextZ21SmartRail2012,
			TextZ21Start2016,
			TextZ21StartLocked,
			TextZ21StartUnlocked,
			TextZ21Type,
			TextZ21Unknown,
			TextZ21White2013,
			MaxTexts
		};

		enum Language : unsigned char
		{
			FirstLanguage = 0,
			EN = 0,
			DE,
			ES,
			MaxLanguages
		};

		static void SetDefaultLanguage(Language language)
		{
			defaultLanguage = language >= MaxLanguages ? EN : language;
		}

		static Language GetDefaultLanguage()
		{
			return defaultLanguage;
		}

		static const char* GetText(const TextSelector selector)
		{
			return GetText(defaultLanguage, selector);
		}

		static const char* GetText(const Language language, const TextSelector selector)
		{
			if (language >= MaxLanguages || selector >= MaxTexts)
			{
				static const char* unknownText = "";
				return unknownText;
			}

			return languages[selector][language];
		}

		static const char* GetOnOff(const bool on)
		{
			return GetText(on ? TextOn : TextOff);
		}

		static const char* GetLeftRight(const Orientation direction)
		{
			return GetText(direction == OrientationRight ? TextRight : TextLeft);
		}

		static const char* GetGreenRed(const DataModel::AccessoryState state)
		{
			return GetText(state == DataModel::AccessoryStateOn ? TextGreen : TextRed);
		}

		static const char* languages[MaxTexts][MaxLanguages];
		static Language defaultLanguage;
};
