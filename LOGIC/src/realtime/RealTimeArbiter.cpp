#include "realtime/RealTimeArbiter.hpp"
#include "model/piece.hpp"
#include "model/piece_removed_info.hpp"

#include <memory>
#include <vector>

void RealTimeArbiter::addMotion(const Motion& motion)
{
    activeMotions.push_back(motion);
}

bool RealTimeArbiter::has_active_motion() const
{
    return !activeMotions.empty();
}

void RealTimeArbiter::addJump(Position cell, int duration)
{
    activeJumps.push_back({
        cell,
        currentTime,
        currentTime + duration
    });
}

bool RealTimeArbiter::isPieceJumping(Position cell) const
{
    for (const auto& jump : activeJumps)
    {
        if (jump.cell.getRow() == cell.getRow() &&
            jump.cell.getCol() == cell.getCol())
        {
            return true;
        }
    }

    return false;
}

bool RealTimeArbiter::isPieceMoving(Position cell) const
{
    for (const auto& motion : activeMotions)
    {
        if (motion.getSource().getRow() == cell.getRow() &&
            motion.getSource().getCol() == cell.getCol())
        {
            return true;
        }
    }

    return false;
}

// פונקציית הכתרה
std::shared_ptr<Piece> RealTimeArbiter::handlePromotion(
    Board& board,
    Position destination,
    std::shared_ptr<Piece> piece)
{
    if (piece && piece->getType() == PieceType::PAWN)
    {
        bool shouldPromote =
            (piece->getColor() == Color::WHITE &&
             destination.getRow() == 0) ||
            (piece->getColor() == Color::BLACK &&
             destination.getRow() == board.rows() - 1);

        if (shouldPromote)
        {
            auto promotedPiece = std::make_shared<Piece>(
                piece->getId(),
                piece->getColor(),
                PieceType::QUEEN,
                destination
            );

            board.setPiece(
                destination.getRow(),
                destination.getCol(),
                promotedPiece
            );

            return promotedPiece;
        }
    }

    return piece;
}

std::vector<PieceRemovedInfo> RealTimeArbiter::advance_time(
    int milliseconds,
    Board& board)
{
    currentTime += milliseconds;

    std::vector<PieceRemovedInfo> removedPieces;
    std::vector<Motion> remainingMotions;

    // טיפול בתנועות הפעילות
    for (const auto& motion : activeMotions)
    {
        if (currentTime < motion.getArrivalTime())
        {
            remainingMotions.push_back(motion);
            continue;
        }

        Position source = motion.getSource();
        Position destination = motion.getDestination();

        auto movingPiece = board.at(
            source.getRow(),
            source.getCol()
        );

        if (!movingPiece)
        {
            continue;
        }

        auto destinationPiece = board.at(
            destination.getRow(),
            destination.getCol()
        );

        // בדיקה האם הכלי במשבצת היעד נמצא בקפיצה
        bool destinationIsJumping = false;

        for (const auto& jump : activeJumps)
        {
            if (jump.cell.getRow() == destination.getRow() &&
                jump.cell.getCol() == destination.getCol() &&
                motion.getArrivalTime() <= jump.arrivalTime)
            {
                destinationIsJumping = true;
                break;
            }
        }

        // הכלי שבתנועה יוצא מהלוח
        if (destinationIsJumping &&
            destinationPiece &&
            movingPiece->getColor() != destinationPiece->getColor())
        {
            removedPieces.push_back({
                movingPiece->getType(),
                movingPiece->getColor(),
                destinationPiece->getColor()
            });

            board.setPiece(
                source.getRow(),
                source.getCol(),
                nullptr
            );

            continue;
        }

        // משבצת היעד מכילה כלי מאותו הצבע
        if (destinationPiece &&
            movingPiece->getColor() == destinationPiece->getColor())
        {
            int rowDifference =
                destination.getRow() - source.getRow();

            int columnDifference =
                destination.getCol() - source.getCol();

            int rowStep =
                rowDifference > 0
                    ? 1
                    : (rowDifference < 0 ? -1 : 0);

            int columnStep =
                columnDifference > 0
                    ? 1
                    : (columnDifference < 0 ? -1 : 0);

            Position newDestination(
                destination.getRow() - rowStep,
                destination.getCol() - columnStep
            );

            if (movingPiece->getType() == PieceType::KNIGHT)
            {
                newDestination = source;
            }
            else
            {
                while (
                    board.hasPiece(newDestination) &&
                    (
                        newDestination.getRow() != source.getRow() ||
                        newDestination.getCol() != source.getCol()
                    )
                )
                {
                    newDestination = Position(
                        newDestination.getRow() - rowStep,
                        newDestination.getCol() - columnStep
                    );
                }
            }

            destination = newDestination;

            destinationPiece = board.at(
                destination.getRow(),
                destination.getCol()
            );
        }

        // כלי מהצד האחר יוצא מהלוח
        if (destinationPiece &&
            movingPiece->getColor() != destinationPiece->getColor())
        {
            removedPieces.push_back({
                destinationPiece->getType(),
                destinationPiece->getColor(),
                movingPiece->getColor()
            });
        }

        // עדכון הלוח
        bool positionChanged =
            source.getRow() != destination.getRow() ||
            source.getCol() != destination.getCol();

        if (positionChanged)
        {
            auto pieceAtDestination = board.at(
                destination.getRow(),
                destination.getCol()
            );

            if (!pieceAtDestination ||
                pieceAtDestination->getColor() != movingPiece->getColor())
            {
                board.movePiece(
                    source,
                    destination
                );
            }
        }

        // בדיקת הכתרה ועדכון מצב הכלי
        auto finalPiece = board.at(
            destination.getRow(),
            destination.getCol()
        );

        finalPiece = handlePromotion(
            board,
            destination,
            finalPiece
        );

        if (finalPiece)
        {
            finalPiece->setState(
                PieceState::long_rest
            );
        }
    }

    activeMotions = remainingMotions;

    // טיפול בסיום קפיצות
    std::vector<ActiveJump> remainingJumps;

    for (const auto& jump : activeJumps)
    {
        if (currentTime < jump.arrivalTime)
        {
            remainingJumps.push_back(jump);
        }
        else
        {
            auto piece = board.at(
                jump.cell.getRow(),
                jump.cell.getCol()
            );

            if (piece)
            {
                piece->setState(
                    PieceState::short_rest
                );
            }
        }
    }

    activeJumps = remainingJumps;

    return removedPieces;
}