#include "realtime/RealTimeArbiter.hpp"
#include <memory>

void RealTimeArbiter::addMotion(const Motion& motion) {
    activeMotions.push_back(motion);
}

bool RealTimeArbiter::has_active_motion() const {
    // אם האוסף אינו ריק, יש תנועה פעילה באוויר
    return !activeMotions.empty();
}
void RealTimeArbiter::addJump(Position cell, int duration) {
    activeJumps.push_back({cell, currentTime + duration});
}

bool RealTimeArbiter::isPieceJumping(Position cell) const {
    for (const auto& j : activeJumps) {
        if (j.cell == cell) {
            return true;
        }
    }
    return false;
}
bool RealTimeArbiter::isPieceMoving(Position cell) const {
    for (const auto& m : activeMotions) {
        if (m.getSource() == cell) {
            return true;
        }
    }
    return false;
}
std::shared_ptr<Piece> RealTimeArbiter::advance_time(int ms, Board& board) {
    currentTime += ms;
    std::shared_ptr<Piece> capturedPiece = nullptr; // כברירת מחדל, כלום לא נאכל

    if (!activeMotions.empty()) {
        const auto& motion = activeMotions.front();
        
        if (currentTime >= motion.getArrivalTime()) {
            Position src = motion.getSource();
            Position dest = motion.getDestination();
            
            auto movingPiece = board.at(src.getRow(), src.getCol());
            auto destPiece = board.at(dest.getRow(), dest.getCol());
            
            bool capturedByAirborne = false;
            // בדיקה האם יש כלי באוויר ביעד באותו זמן הגעה
            for (const auto& jump : activeJumps) {
                if (jump.cell == dest && motion.getArrivalTime() <= jump.arrivalTime) {
                    capturedByAirborne = true;
                    break;
                }
            }

            // אם יש כלי באוויר והם אויבים - הכלי באוויר אוכל את הכלי המגיע
            if (capturedByAirborne && destPiece != nullptr && movingPiece->getColor() != destPiece->getColor()) {
                capturedPiece = movingPiece;
                board.setPiece(src.getRow(), src.getCol(), nullptr); // הסרת הכלי המגיע שהתקיף
            } else {
                capturedPiece = destPiece;
                board.movePiece(src, dest);

                auto finalPiece = board.at(dest.getRow(), dest.getCol());
                if (finalPiece->getType() == PieceType::PAWN && finalPiece->getColor() == Color::WHITE && dest.getRow() == 0) {
                    auto queen = std::make_shared<Piece>(finalPiece->getId(), Color::WHITE, PieceType::QUEEN, dest);
                    board.setPiece(dest.getRow(), dest.getCol(), queen);
                } else if (finalPiece->getType() == PieceType::PAWN && finalPiece->getColor() == Color::BLACK && dest.getRow() == board.rows() - 1) {
                    auto queen = std::make_shared<Piece>(finalPiece->getId(), Color::BLACK, PieceType::QUEEN, dest);
                    board.setPiece(dest.getRow(), dest.getCol(), queen);
                }
            }
            activeMotions.clear();
        }
    }

    // ניקוי קפיצות שחלפו/נחתו
    std::vector<ActiveJump> remainingJumps;
    for (const auto& jump : activeJumps) {
        if (currentTime < jump.arrivalTime) {
            remainingJumps.push_back(jump);
        }
    }
    activeJumps = remainingJumps;
    
    return capturedPiece; // מדווח מי נאכל[
}