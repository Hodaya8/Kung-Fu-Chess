#include "realtime/RealTimeArbiter.hpp"
#include "model/piece.hpp" 
#include <memory>

void RealTimeArbiter::addMotion(const Motion& motion) { activeMotions.push_back(motion); }

bool RealTimeArbiter::has_active_motion() const { return !activeMotions.empty(); }

void RealTimeArbiter::addJump(Position cell, int duration) { activeJumps.push_back({cell, currentTime + duration}); }

bool RealTimeArbiter::isPieceJumping(Position cell) const {
    for (const auto& j : activeJumps) { 
        if (j.cell.getRow() == cell.getRow() && j.cell.getCol() == cell.getCol()) return true; 
    }
    return false;
}

bool RealTimeArbiter::isPieceMoving(Position cell) const {
    for (const auto& m : activeMotions) { 
        if (m.getSource().getRow() == cell.getRow() && m.getSource().getCol() == cell.getCol()) return true; 
    }
    return false;
}

//פונקציית הכתרה
std::shared_ptr<Piece> RealTimeArbiter::handlePromotion(Board& board, Position dest, std::shared_ptr<Piece> piece) {
    if (piece && piece->getType() == PieceType::PAWN) {
        bool promote = (piece->getColor() == Color::WHITE && dest.getRow() == 0) || 
                       (piece->getColor() == Color::BLACK && dest.getRow() == board.rows() - 1);
        if (promote) {
            auto promotedPiece = std::make_shared<Piece>(piece->getId(), piece->getColor(), PieceType::QUEEN, dest);
            board.setPiece(dest.getRow(), dest.getCol(), promotedPiece);
            return promotedPiece; // החזרת המלכה החדשה
        }
    }
    return piece; // אם אין הכתרה, מחזירים את הכלי כפי שהוא
}

std::shared_ptr<Piece> RealTimeArbiter::advance_time(int ms, Board& board) {
    currentTime += ms;
    std::shared_ptr<Piece> capturedPiece = nullptr; 
    std::vector<Motion> remainingMotions;

    // --- טיפול בכלים שבאוויר ---
    for (const auto& motion : activeMotions) {
        if (currentTime < motion.getArrivalTime()) {
            remainingMotions.push_back(motion); 
            continue;
        }

        Position src = motion.getSource();
        Position dest = motion.getDestination();
        auto movingPiece = board.at(src.getRow(), src.getCol());
        auto destPiece = board.at(dest.getRow(), dest.getCol());
        
        // בדיקת התחמקות של כלי שקפץ ביעד
        bool capturedByAirborne = false;
        for (const auto& jump : activeJumps) {
            if (jump.cell.getRow() == dest.getRow() && jump.cell.getCol() == dest.getCol() && motion.getArrivalTime() <= jump.arrivalTime) {
                capturedByAirborne = true;
                break;
            }
        }

        // ביטול המהלך אם נאכל באוויר על ידי אויב
        if (capturedByAirborne && destPiece && movingPiece && movingPiece->getColor() != destPiece->getColor()) {
            capturedPiece = movingPiece;
            board.setPiece(src.getRow(), src.getCol(), nullptr); 
            continue; 
        }

        // התנגשות עם חבר (אותו צבע)
        if (destPiece && movingPiece && movingPiece->getColor() == destPiece->getColor()) {
            int dr = dest.getRow() - src.getRow();
            int dc = dest.getCol() - src.getCol();
            int stepRow = (dr > 0) ? 1 : ((dr < 0) ? -1 : 0);
            int stepCol = (dc > 0) ? 1 : ((dc < 0) ? -1 : 0);
            
            Position newDest(dest.getRow() - stepRow, dest.getCol() - stepCol);
            
            if (movingPiece->getType() == PieceType::KNIGHT) {
                newDest = src; // פרש חוזר למקור
            } else {
                while (board.hasPiece(newDest) && (newDest.getRow() != src.getRow() || newDest.getCol() != src.getCol())) {
                    newDest = Position(newDest.getRow() - stepRow, newDest.getCol() - stepCol);
                }
            }
            dest = newDest;
            destPiece = board.at(dest.getRow(), dest.getCol()); 
        }

        // נחיתה או אכילה רגילה
        if (destPiece && movingPiece && movingPiece->getColor() != destPiece->getColor()) {
            capturedPiece = destPiece;
        }
        
        // הזזת הכלי פיזית
        if (src.getRow() != dest.getRow() || src.getCol() != dest.getCol()) {
            auto sanityCheck = board.at(dest.getRow(), dest.getCol());
            if (sanityCheck == nullptr || sanityCheck->getColor() != movingPiece->getColor()) {
                board.movePiece(src, dest);
            }
        }

        // הכתרה (בקריאה אלגנטית אחת) + עונש מנוחה
        auto finalPiece = board.at(dest.getRow(), dest.getCol());
        finalPiece = handlePromotion(board, dest, finalPiece);
        
        if (finalPiece) finalPiece->setState(PieceState::long_rest);
    }
    
    activeMotions = remainingMotions;

    // --- טיפול בסיום קפיצות ---
    std::vector<ActiveJump> remainingJumps;
    for (const auto& jump : activeJumps) {
        if (currentTime < jump.arrivalTime) {
            remainingJumps.push_back(jump);
        } else {
            auto piece = board.at(jump.cell.getRow(), jump.cell.getCol());
            if (piece) piece->setState(PieceState::short_rest);
        }
    }
    activeJumps = remainingJumps;
    
    return capturedPiece; 
}