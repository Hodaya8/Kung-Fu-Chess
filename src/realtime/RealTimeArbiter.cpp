#include "realtime/RealTimeArbiter.hpp"

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
std::string RealTimeArbiter::advance_time(int ms, Board& board) {
    currentTime += ms;
    std::string capturedPiece = ""; // כברירת מחדל, כלום לא נאכל

    if (!activeMotions.empty()) {
        const auto& motion = activeMotions.front();
        
        if (currentTime >= motion.getArrivalTime()) {
            Position src = motion.getSource();
            Position dest = motion.getDestination();
            
            std::string movingPieceName = board.at(src.getRow(), src.getCol());
            std::string destPieceName = board.at(dest.getRow(), dest.getCol());
            
            bool capturedByAirborne = false;
            // בדיקה האם יש כלי באוויר ביעד באותו זמן הגעה
            for (const auto& jump : activeJumps) {
                if (jump.cell == dest && motion.getArrivalTime() <= jump.arrivalTime) {
                    capturedByAirborne = true;
                    break;
                }
            }

            // אם יש כלי באוויר והם אויבים - הכלי באוויר אוכל את הכלי המגיע
            if (capturedByAirborne && destPieceName != "." && movingPieceName[0] != destPieceName[0]) {
                capturedPiece = movingPieceName;
                board.setPiece(src.getRow(), src.getCol(), "."); // הסרת הכלי המגיע שהתקיף
            } else {
                capturedPiece = destPieceName;
                board.movePiece(src, dest);

                std::string finalPiece = board.at(dest.getRow(), dest.getCol());
                if (finalPiece == "wP" && dest.getRow() == 0) {
                    board.setPiece(dest.getRow(), dest.getCol(), "wQ");
                } else if (finalPiece == "bP" && dest.getRow() == board.rows() - 1) {
                    board.setPiece(dest.getRow(), dest.getCol(), "bQ");
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