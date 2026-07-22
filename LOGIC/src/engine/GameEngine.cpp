// #include "engine/GameEngine.hpp"
// #include "RealTimeArbiter.hpp"
// #include "Motion.hpp"
// #include "game_snapshot.hpp"
// #include "model/piece.hpp"
// #include "model/game_snapshot.hpp"
// #include <unordered_set>
// #include <algorithm>

// //constructor
// GameEngine::GameEngine(Board& board) : board(board), game_over(false) {}

// //פונקציית קידום הזמן ובדיקת סיום משחק
// void GameEngine::wait(int ms) {
//     auto capturedPiece = arbiter.advance_time(ms, board);    //במידה ונאכל כלי הוא מוחזר
//     //בדיקת סיום משחק
//     if (capturedPiece && capturedPiece->getType() == PieceType::KING) {
//         game_over = true; 
//     }
// }

// //פונקצית בדיקת חוקיות תנועה-בקשת תנועה
// void GameEngine::requestMove(Position source, Position destination) {
//     if (game_over || arbiter.isPieceMoving(source)) return;
//     auto piece = board.at(source.getRow(), source.getCol());
//     if (!piece || piece->getState() == PieceState::short_rest || piece->getState() == PieceState::long_rest) return; 
//     if (ruleEngine.isLegalMove(piece, source, destination, board)) {
//         //הוספה למערך התנועות
//         arbiter.addMotion(Motion(source, destination, arbiter.getCurrentTime() + ruleEngine.getTravelTime(piece, source, destination)));
//     }
// }

// // פונקציית בקשת קפיצה
// void GameEngine::requestJump(Position cell) {
//     if (game_over || arbiter.isPieceMoving(cell)) return;
//     auto piece = board.at(cell.getRow(), cell.getCol());
//     if (!piece || piece->getState() == PieceState::short_rest || piece->getState() == PieceState::long_rest) return; 
//     //הוספה למערך הקפיצות
//     arbiter.addJump(cell, 300);
// }

// // bool GameEngine::hasActiveMotion() const {
// //     return arbiter.has_active_motion();
// // }

// GameSnapshot GameEngine::getSnapshot() const {
//     std::vector<PieceSnapshot> snapshots;
//     addStaticSnapshots(snapshots);
//     addJumpingSnapshots(snapshots); 
//     addMovingSnapshots(snapshots);
//     return GameSnapshot(snapshots);
// }

// void GameEngine::addStaticSnapshots(std::vector<PieceSnapshot>& snapshots) const {
//     int currentTime = arbiter.getCurrentTime();
//     for (int r = 0; r < board.rows(); ++r) {
//         for (int c = 0; c < board.cols(); ++c) {
//             auto piece = board.at(r, c);
//             if (!piece || arbiter.isPieceMoving(Position(r, c)) || arbiter.isPieceJumping(Position(r, c))) continue;
//             //יצירת אובייקט לכלי נח
//             PieceSnapshot snap;
//             snap.type = piece->toString();
//             snap.row = static_cast<double>(r);
//             snap.col = static_cast<double>(c);
//             snap.state = piece->getState(); 
//             snap.timeRemaining = 0;
//             //יצירת האנימציה
//             snap.animationProgress = (snap.state == PieceState::idle) ? (double)(currentTime % 500) / 500.0 : 0.0;
//             //טיפול במצב מנוחה
//             handleRestState(snap, piece); 
//             //הוספה למערך הדינאמי
//             snapshots.push_back(snap);
//         }
//     }
// }

// void GameEngine::addMovingSnapshots(std::vector<PieceSnapshot>& snapshots) const {
//     int currentTime = arbiter.getCurrentTime();
//     // מעבר על הכלים שבתנועה
//     for (const auto& m : arbiter.getActiveMotions()) {
//         auto piece = board.at(m.getDestination().getRow(), m.getDestination().getCol());
//         if (!piece) piece = board.at(m.getSource().getRow(), m.getSource().getCol());
//         if (!piece) continue;
//         //יצירת אובייקט לכלי נע
//         PieceSnapshot snap;
//         snap.type = piece->toString();
//         snap.state = PieceState::move;

//         double progress = 1.0 - (static_cast<double>(m.getArrivalTime() - currentTime) / 1000.0);
//         snap.animationProgress = std::clamp(progress, 0.0, 1.0);
//         //חישוב המיקום המדויק על הלוח
//         snap.row = m.getSource().getRow() + (m.getDestination().getRow() - m.getSource().getRow()) * snap.animationProgress;
//         snap.col = m.getSource().getCol() + (m.getDestination().getCol() - m.getSource().getCol()) * snap.animationProgress;
//         //הוספה למערך הדינאמי
//         snapshots.push_back(snap);
//     }
// }

// void GameEngine::addJumpingSnapshots(std::vector<PieceSnapshot>& snapshots) const {
//     int currentTime = arbiter.getCurrentTime();
//     for (int r = 0; r < board.rows(); ++r) {
//         for (int c = 0; c < board.cols(); ++c) {
//             auto piece = board.at(r, c);
            
//             if (!piece || !arbiter.isPieceJumping(Position(r, c))) continue;

//             PieceSnapshot snap;
//             snap.type = piece->toString();
//             snap.state = PieceState::jump;
//             snap.row = static_cast<double>(r);
//             snap.col = static_cast<double>(c);
//             snap.timeRemaining = 0;
//             snap.animationProgress = (double)(currentTime % 200) / 200.0; // מהירות של 200

//             snapshots.push_back(snap);
//         }
//     }
// }

// void GameEngine::handleRestState(PieceSnapshot& snap, std::shared_ptr<Piece> piece) const {
//     if (snap.state != PieceState::long_rest && snap.state != PieceState::short_rest) return;
//     int duration = (snap.state == PieceState::long_rest) ? 2000 : 1000;
//     int pId = piece->getId();
//     int currentTime = arbiter.getCurrentTime();
    
//     if (restStartTimes.find(pId) == restStartTimes.end()) restStartTimes[pId] = currentTime;
    
//     int elapsed = currentTime - restStartTimes.at(pId);   // הזמן שעבר מתחילת המנוחה
//     snap.timeRemaining = std::max(0, duration - elapsed);   // הזמן שנותר
//     snap.animationProgress = static_cast<double>(elapsed) / duration;   // חישוב אחוז ההתקדמות של המנוחה - להצגת מספר 
//     // שחרור הכלי בסיום הזמן
//     if (snap.timeRemaining == 0) {
//         const_cast<Piece*>(piece.get())->setState(PieceState::idle);
//         restStartTimes.erase(pId);
//         snap.state = PieceState::idle;
//     }
// }


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

void GameEngine::wait(int milliseconds)
{
    auto capturedPiece =
        arbiter.advance_time(milliseconds, board);

    if (ruleEngine.isGameOver(capturedPiece))
    {
        game_over = true;
    }
}

void GameEngine::requestMove(
    Position source,
    Position destination)
{
    if (game_over || arbiter.isPieceMoving(source))
    {
        return;
    }

    auto piece = board.at(
        source.getRow(),
        source.getCol()
    );

    if (!piece ||
        piece->getState() == PieceState::short_rest ||
        piece->getState() == PieceState::long_rest)
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

void GameEngine::requestJump(Position cell)
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
        piece->getState() == PieceState::short_rest ||
        piece->getState() == PieceState::long_rest)
    {
        return;
    }

    arbiter.addJump(
        cell,
        JUMP_DURATION_MS
    );
}

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
    for (int row = 0; row < board.rows(); ++row)
    {
        for (int col = 0; col < board.cols(); ++col)
        {
            Position position(row, col);
            auto piece = board.at(row, col);

            if (!piece ||
                arbiter.isPieceMoving(position) ||
                arbiter.isPieceJumping(position))
            {
                continue;
            }

            PieceSnapshot snapshot;

            snapshot.type = piece->toString();
            snapshot.state = piece->getState();

            snapshot.row = row;
            snapshot.col = col;

            snapshot.destinationRow = row;
            snapshot.destinationCol = col;

            handleRestState(snapshot, piece);

            snapshots.push_back(snapshot);
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

        snapshot.type = piece->toString();
        snapshot.state = PieceState::move;

        snapshot.row = source.getRow();
        snapshot.col = source.getCol();

        snapshot.destinationRow =
            destination.getRow();

        snapshot.destinationCol =
            destination.getCol();

        snapshot.actionStartTime =
            motion.getStartTime();

        snapshot.actionEndTime =
            motion.getArrivalTime();

        snapshots.push_back(snapshot);
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

        snapshot.type = piece->toString();
        snapshot.state = PieceState::jump;

        snapshot.row = jump.cell.getRow();
        snapshot.col = jump.cell.getCol();

        snapshot.destinationRow =
            jump.cell.getRow();

        snapshot.destinationCol =
            jump.cell.getCol();

        snapshot.actionStartTime =
            jump.startTime;

        snapshot.actionEndTime =
            jump.arrivalTime;

        snapshots.push_back(snapshot);
    }
}

void GameEngine::handleRestState(
    PieceSnapshot& snapshot,
    std::shared_ptr<Piece> piece) const
{
    if (snapshot.state != PieceState::long_rest &&
        snapshot.state != PieceState::short_rest)
    {
        return;
    }

    const int duration =
        snapshot.state == PieceState::long_rest
            ? LONG_REST_DURATION_MS
            : SHORT_REST_DURATION_MS;

    const int pieceId = piece->getId();
    const int currentTime =
        arbiter.getCurrentTime();

    if (restStartTimes.find(pieceId) ==
        restStartTimes.end())
    {
        restStartTimes[pieceId] = currentTime;
    }

    const int elapsed =
        currentTime - restStartTimes.at(pieceId);

    snapshot.stateDuration = duration;

    snapshot.timeRemaining =
        std::max(0, duration - elapsed);

    if (snapshot.timeRemaining == 0)
    {
        piece->setState(PieceState::idle);
        restStartTimes.erase(pieceId);

        snapshot.state = PieceState::idle;
        snapshot.stateDuration = 0;
        snapshot.timeRemaining = -1;
    }
}

bool GameEngine::hasActiveMotion() const
{
    return arbiter.has_active_motion();
}