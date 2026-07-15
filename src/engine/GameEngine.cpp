#include "engine/GameEngine.hpp"
#include "realtime/RealTimeArbiter.hpp"
#include "realtime/Motion.hpp"

GameEngine::GameEngine(Board& board)
:
board(board), game_over(false)
{
}


void GameEngine::wait(int ms)
{
    // המנוע מקבל דיווח על אכילה מה-Arbiter
    std::string capturedPiece = arbiter.advance_time(ms, board);
    handlePromotion();
    // המנוע מפרש את הדיווח: האם זה מלך?
    if (capturedPiece == "wK" || capturedPiece == "bK") {
        game_over = true; // המנוע הוא זה שמחליט שהמשחק נגמר
    }
}

void GameEngine::requestMove(Position source, Position destination)
{
    if (game_over) return;

    // חוק המסלול המשותף: דחיית בקשה אם כבר יש תנועה באוויר
    if (arbiter.has_active_motion()) {
        return;
    }

    const std::string& piece = board.at(source.getRow(), source.getCol());
    if (piece == ".") return; // אין כלי במשבצת המקור

    // אם הלוח פנוי והמהלך חוקי לפי החוקים הכלליים של הכלים
    if (ruleEngine.isLegalMove(piece, source, destination, board))
    {
        int travelTime = ruleEngine.getTravelTime(piece, source, destination);
        int arrivalTime = arbiter.getCurrentTime() + travelTime;

        // רושמים את התנועה (מה שיחסום תנועות אחרות עד שהזמן יעבור)
        arbiter.addMotion(Motion(source, destination, arrivalTime));
    }
}

void GameEngine::requestJump(Position cell)
{
    if (game_over) return;

    // חוק 1: כלי שנאכל (לא קיים פיזית על הלוח) לא יכול לקפוץ
    if (!board.hasPiece(cell)) return;

    // חוק 2: כלי שנמצא כרגע באמצע תנועה (כלומר, הוא המקור של Motion פעיל) לא יכול לקפוץ
    if (arbiter.isPieceMoving(cell)) return;

    // אם הכל תקין, רושמים את הקפיצה בבורר למשך 1000 מילישניות (שנייה אחת)
    arbiter.addJump(cell, 1000);
}

void GameEngine::handlePromotion()
{
    int lastRow = board.rows() - 1;
    for (int col = 0; col < board.cols(); ++col) {
        if (board.at(0, col) == "wP") {
            board.setPiece(0, col, "wQ");
        }
    }
    for (int col = 0; col < board.cols(); ++col) {
        if (board.at(lastRow, col) == "bP") {
            board.setPiece(lastRow, col, "bQ");
        }
    }
}