#include "engine/GameEngine.hpp"

#include <algorithm>

#include "realtime/Motion.hpp"

namespace
{
    constexpr int JUMP_DURATION_MS = 300;
    constexpr int SHORT_REST_DURATION_MS = 1000;
    constexpr int LONG_REST_DURATION_MS = 2000;
}

GameEngine::GameEngine(Board& board)
    : board(board),
      game_over(false)
{
}

// קידום הזמן ובדיקת סיום המשחק
std::vector<PieceRemovedInfo>
GameEngine::wait(int milliseconds)
{
    auto removedPieces =
        arbiter.advance_time(
            milliseconds,
            board
        );

    for (const auto& removedPiece :
         removedPieces)
    {
        if (ruleEngine.isGameOver(
                removedPiece.removedType))
        {
            game_over = true;

            winnerColor =
                removedPiece.scoringColor;

            break;
        }
    }

    return removedPieces;
}

// בקשת תנועה
void GameEngine::requestMove(
    Position source,
    Position destination)
{
    if (game_over ||
        arbiter.isPieceMoving(source))
    {
        return;
    }

    auto piece = board.at(
        source.getRow(),
        source.getCol()
    );

    if (!piece ||
        piece->getState() ==
            PieceState::short_rest ||
        piece->getState() ==
            PieceState::long_rest)
    {
        return;
    }

    if (!ruleEngine.isLegalMove(
            piece,
            source,
            destination,
            board))
    {
        return;
    }

    const int startTime =
        arbiter.getCurrentTime();

    const int travelTime =
        ruleEngine.getTravelTime(
            piece,
            source,
            destination
        );

    const int arrivalTime =
        startTime + travelTime;

    arbiter.addMotion(
        Motion(
            source,
            destination,
            startTime,
            arrivalTime
        )
    );
}

// בקשת קפיצה
void GameEngine::requestJump(
    Position cell)
{
    if (game_over ||
        arbiter.isPieceMoving(cell))
    {
        return;
    }

    auto piece = board.at(
        cell.getRow(),
        cell.getCol()
    );

    if (!piece ||
        piece->getState() ==
            PieceState::short_rest ||
        piece->getState() ==
            PieceState::long_rest)
    {
        return;
    }

    arbiter.addJump(
        cell,
        JUMP_DURATION_MS
    );
}

// יצירת תמונת מצב של המשחק
GameSnapshot GameEngine::getSnapshot() const
{
    std::vector<PieceSnapshot> snapshots;

    addStaticSnapshots(snapshots);
    addJumpingSnapshots(snapshots);
    addMovingSnapshots(snapshots);

    return GameSnapshot(
        snapshots,
        arbiter.getCurrentTime()
    );
}

void GameEngine::addStaticSnapshots(
    std::vector<PieceSnapshot>& snapshots) const
{
    for (int row = 0;
         row < board.rows();
         ++row)
    {
        for (int col = 0;
             col < board.cols();
             ++col)
        {
            Position position(
                row,
                col
            );

            auto piece =
                board.at(
                    row,
                    col
                );

            if (!piece ||
                arbiter.isPieceMoving(
                    position
                ) ||
                arbiter.isPieceJumping(
                    position
                ))
            {
                continue;
            }

            PieceSnapshot snapshot;

            snapshot.type =
                piece->toString();

            snapshot.state =
                piece->getState();

            snapshot.row = row;
            snapshot.col = col;

            snapshot.destinationRow =
                row;

            snapshot.destinationCol =
                col;

            handleRestState(
                snapshot,
                piece
            );

            snapshots.push_back(
                snapshot
            );
        }
    }
}

void GameEngine::addMovingSnapshots(
    std::vector<PieceSnapshot>& snapshots) const
{
    for (const auto& motion :
         arbiter.getActiveMotions())
    {
        const Position source =
            motion.getSource();

        const Position destination =
            motion.getDestination();

        auto piece = board.at(
            source.getRow(),
            source.getCol()
        );

        if (!piece)
        {
            piece = board.at(
                destination.getRow(),
                destination.getCol()
            );
        }

        if (!piece)
        {
            continue;
        }

        PieceSnapshot snapshot;

        snapshot.type =
            piece->toString();

        snapshot.state =
            PieceState::move;

        snapshot.row =
            source.getRow();

        snapshot.col =
            source.getCol();

        snapshot.destinationRow =
            destination.getRow();

        snapshot.destinationCol =
            destination.getCol();

        snapshot.actionStartTime =
            motion.getStartTime();

        snapshot.actionEndTime =
            motion.getArrivalTime();

        snapshots.push_back(
            snapshot
        );
    }
}

void GameEngine::addJumpingSnapshots(
    std::vector<PieceSnapshot>& snapshots) const
{
    for (const auto& jump :
         arbiter.getActiveJumps())
    {
        auto piece = board.at(
            jump.cell.getRow(),
            jump.cell.getCol()
        );

        if (!piece)
        {
            continue;
        }

        PieceSnapshot snapshot;

        snapshot.type =
            piece->toString();

        snapshot.state =
            PieceState::jump;

        snapshot.row =
            jump.cell.getRow();

        snapshot.col =
            jump.cell.getCol();

        snapshot.destinationRow =
            jump.cell.getRow();

        snapshot.destinationCol =
            jump.cell.getCol();

        snapshot.actionStartTime =
            jump.startTime;

        snapshot.actionEndTime =
            jump.arrivalTime;

        snapshots.push_back(
            snapshot
        );
    }
}

void GameEngine::handleRestState(
    PieceSnapshot& snapshot,
    std::shared_ptr<Piece> piece) const
{
    if (snapshot.state !=
            PieceState::long_rest &&
        snapshot.state !=
            PieceState::short_rest)
    {
        return;
    }

    const int duration =
        snapshot.state ==
            PieceState::long_rest
                ? LONG_REST_DURATION_MS
                : SHORT_REST_DURATION_MS;

    const int pieceId =
        piece->getId();

    const int currentTime =
        arbiter.getCurrentTime();

    if (restStartTimes.find(
            pieceId) ==
        restStartTimes.end())
    {
        restStartTimes[pieceId] =
            currentTime;
    }

    const int elapsed =
        currentTime -
        restStartTimes.at(
            pieceId
        );

    snapshot.stateDuration =
        duration;

    snapshot.timeRemaining =
        std::max(
            0,
            duration - elapsed
        );

    if (snapshot.timeRemaining == 0)
    {
        piece->setState(
            PieceState::idle
        );

        restStartTimes.erase(
            pieceId
        );

        snapshot.state =
            PieceState::idle;

        snapshot.stateDuration = 0;
        snapshot.timeRemaining = -1;
    }
}