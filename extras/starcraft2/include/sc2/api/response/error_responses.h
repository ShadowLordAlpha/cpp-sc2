/**
 *
 */

#pragma once

namespace astra::sc2
{
    struct ErrorInfo
    {
        enum class Error
        {
            UnknownError,
            TransportError,
            Timeout,

            // CreateGame errors
            MissingMap,
            InvalidMapPath,
            InvalidMapData,
            InvalidMapName,
            InvalidMapHandle,
            MissingPlayerSetup,
            InvalidPlayerSetup,
            MultiplayerUnsupported,

            // JoinGame errors
            MissingParticipation,
            InvalidObservedPlayerId,
            MissingOptions,
            MissingPorts,
            GameFull,
            LaunchError,
            FeatureUnsupported,
            NoSpaceForUser,
            MapDoesNotExist,
            CannotOpenMap,
            ChecksumError,
            NetworkError,
            OtherError,

            // RestartGame errors
            // LaunchError, // This is already provided by JoinGame

            // StartReplay errors
            MissingReplay,
            InvalidReplayPath,
            InvalidReplayData,
            ParsingError,
            // LaunchError,


            // MapCommand errors
            NoTriggerError,

            // ReplayInfo errors
            // MissingReplay,
            // InvalidReplayPath,
            // InvalidReplayData,
            // ParsingError,
            DownloadError,

            // SaveMap errors
            // InvalidMapData,
        };

        const Error error;
        std::string message;

        static Error convertReplayInfoError(int error)
        {
            switch (error) {
                case 1:
                    return Error::MissingReplay;
                case 2:
                    return Error::InvalidReplayPath;
                case 3:
                    return Error::InvalidReplayData;
                case 4:
                    return Error::ParsingError;
                case 5:
                    return Error::DownloadError;
                default:
                    return Error::UnknownError;
            }
        }


    };
}
