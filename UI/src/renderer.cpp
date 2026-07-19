#include "renderer.hpp"
#include <iostream>
#include "engine/GameEngine.hpp"
#include "input/Controller.hpp" 

//הכנת הניתוב לתמונות הכלים - בנאי
Renderer::Renderer(const std::string& assetsDir) : cellSize(100) {
    boardPath = assetsDir + "/board_classic.png"; 
    std::string basePath = assetsDir + "/pieces_classic/pieces_classic/";
    const char pieceTypes[] = {'Q', 'K', 'R', 'B', 'N', 'P'};
    for (char type : pieceTypes) {
        piecePaths[std::string("w") + type] = basePath + type + "W";
        piecePaths[std::string("b") + type] = basePath + type + "B";
    }
}

// פונקציה לציור המשחק
void Renderer::render(const GameSnapshot& snapshot, const GameEngine& gameEngine, const Controller& controller) {
    try {
        static const std::map<PieceState, std::string> stateToString = {
            {PieceState::idle, "idle"}, {PieceState::move, "move"},
            {PieceState::jump, "jump"}, {PieceState::long_rest, "long_rest"},
            {PieceState::short_rest, "short_rest"}, {PieceState::captured, "captured"}
        };

        Img frame;   // אובייקט המכיל את תמונת הלוח
        frame.read(boardPath, std::make_pair(8 * cellSize, 8 * cellSize), false);
        //מעבר על הכלים
        for (const auto& pieceSnap : snapshot.getPieces()) {
            if (piecePaths.find(pieceSnap.type) != piecePaths.end()) {
                //בדיקת מצב הכלי
                std::string stateStr = stateToString.at(pieceSnap.state);

                int totalFrames = 5;
                int currentFrame = 1 + static_cast<int>(pieceSnap.animationProgress * (totalFrames - 1));
                //טעינת התמונה
                std::string currentImgPath = piecePaths[pieceSnap.type] + "/states/" + stateStr + "/sprites/" + std::to_string(currentFrame) + ".png";
                //אובייקט לתמונת האנימציה
                Img pieceImg;
                try {
                    pieceImg.read(currentImgPath, std::make_pair(cellSize, cellSize), true);
                } catch (...) {
                    std::string fallbackPath = piecePaths[pieceSnap.type] + "/states/idle/sprites/1.png";
                    pieceImg.read(fallbackPath, std::make_pair(cellSize, cellSize), true);
                }
                //חישוב נקודת ציון של הכלי
                int drawX = static_cast<int>(pieceSnap.col * cellSize);
                int drawY = static_cast<int>(pieceSnap.row * cellSize);
                pieceImg.draw_on(frame, drawX, drawY);
                //סימון בחירת כלי
                if (controller.isSelected()) {
                    Position selectedPos = controller.getSelected();
                    if (static_cast<int>(pieceSnap.row) == selectedPos.getRow() && 
                        static_cast<int>(pieceSnap.col) == selectedPos.getCol()) {
                        
                        cv::rectangle(frame.get_mat(), 
                                      cv::Point(drawX, drawY), 
                                      cv::Point(drawX + cellSize, drawY + cellSize), 
                                      cv::Scalar(0, 255, 255), 4); // צהוב, עובי 4
                    }
                }
                //הצגת זמן נותר למנוחה
                if (pieceSnap.timeRemaining >= 0) {
                    std::string timeText = std::to_string(pieceSnap.timeRemaining / 100);
                    cv::putText(frame.get_mat(), timeText, cv::Point(drawX + 10, drawY + 30), 
                                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 255), 2);
                }
            }
        }
        //סיום משחק
        if (gameEngine.isGameOver()) {
            cv::rectangle(frame.get_mat(), cv::Point(0, 0), cv::Point(800, 800), cv::Scalar(0, 0, 0), -1);
            cv::putText(frame.get_mat(), "GAME OVER", cv::Point(150, 400), cv::FONT_HERSHEY_DUPLEX, 3, cv::Scalar(0, 0, 255), 5);
        }
        //הצגת כל הנתונים על גבי המסך
        cv::imshow("Image", frame.get_mat());
    } catch (const std::exception& e) {
        std::cerr << "Render error: " << e.what() << std::endl;
    }
}