#include "engine/GameEngine.hpp"
#include "realtime/RealTimeArbiter.hpp"
#include "realtime/Motion.hpp"

GameEngine::GameEngine(Board& board)
:
board(board), game_over(false)
{
}

//מקדמת שעון מדומה
void GameEngine::wait(int ms)
{
    //מתקבל דיווח אם מישהו נאכך ונבדק אם זה המלך שאז נגמר המשחר
    auto capturedPiece = arbiter.advance_time(ms, board);

    if (capturedPiece != nullptr &&capturedPiece->getType() == PieceType::KING) {
        game_over = true; 
    }
}



void GameEngine::requestMove(Position source, Position destination)
{
    if (game_over||arbiter.has_active_motion()) return;
    
    auto piece = board.at(source.getRow(), source.getCol());
    
    if (piece == nullptr) return; // אין כלי במשבצת המקור

    // אם הלוח פנוי והמהלך חוקי     
    if (ruleEngine.isLegalMove(piece, source, destination, board))
    {
        int travelTime = ruleEngine.getTravelTime(piece, source, destination);
        int arrivalTime = arbiter.getCurrentTime() + travelTime;

        // רושמים את התנועה (מה שיחסום תנועות אחרות עד שהזמן יעבור)
        arbiter.addMotion(Motion(source, destination, arrivalTime));
    }
}

//פונקציית קפיצת כלי
void GameEngine::requestJump(Position cell)
{
    if (game_over) return;

    if (!board.hasPiece(cell)) return;

    if (arbiter.isPieceMoving(cell)) return;

    arbiter.addJump(cell, 1000);
}

//הכתרת חייל למלכה
void GameEngine::handlePromotion()
{
    int lastRow = board.rows() - 1;
    //חייל לבן
    for (int col = 0; col < board.cols(); ++col) {
        auto pWhite = board.at(0, col);
        if (pWhite && pWhite->getType() == PieceType::PAWN && pWhite->getColor() == Color::WHITE) {
            auto queen = std::make_shared<Piece>(pWhite->getId(), Color::WHITE, PieceType::QUEEN, Position(0, col));
            board.setPiece(0, col, queen);
        }
    //חייל שחור
    auto pBlack = board.at(lastRow, col);
        if (pBlack && pBlack->getType() == PieceType::PAWN && pBlack->getColor() == Color::BLACK) {
            auto queen = std::make_shared<Piece>(pBlack->getId(), Color::BLACK, PieceType::QUEEN, Position(lastRow, col));
            board.setPiece(lastRow, col, queen);
        }
    }
}