#include <iostream>
#include <opencv2/opencv.hpp>
#include "Board.hpp"
#include "GameEngine.hpp"
#include "Renderer.hpp"
#include "input/Controller.hpp"
#include "model/Board.hpp"
#include "io/board_parser.hpp"

// משתנים גלובליים לחיבור ה-UI עם הקונטרולר
Controller* g_controller = nullptr;
GameEngine* g_engine = nullptr;
Renderer* g_renderer = nullptr;

// פונקציית הקליק - משתמשת אך ורק במצביעים הגלובליים!
// פונקציית הקליק - משתמשת אך ורק במצביעים הגלובליים!
void onMouseClick(int event, int x, int y, int flags, void* userdata) {
    bool stateChanged = false;
    
    // קליק שמאלי - תנועה/בחירה
    if (event == cv::EVENT_LBUTTONDOWN) {
        g_controller->click(x, y);
        stateChanged = true;
    }
    // קליק ימני - קפיצה
    else if (event == cv::EVENT_RBUTTONDOWN) {
        g_controller->jump(x, y);
        stateChanged = true;
    }

    // ציור מחדש רק אם היה שינוי
    if (stateChanged) {
        GameSnapshot snap = g_engine->getSnapshot();
        g_renderer->render(snap, *g_engine, *g_controller);
    }
}

int main() {
    try {
        // --- 1. אתחול הלוגיקה ---
        std::string initialBoard = 
            "Board:\n"
            "bR bN bB bQ bK bB bN bR\n"
            "bP bP bP bP bP bP bP bP\n"
            ". . . . . . . .\n"
            ". . . . . . . .\n"
            ". . . . . . . .\n"
            ". . . . . . . .\n"
            "wP wP wP wP wP wP wP wP\n"
            "wR wN wB wQ wK wB wN wR\n";
        
        std::stringstream ss(initialBoard);
        Board board = BoardParser::parse(ss);
        GameEngine engine(board);
        BoardMapper mapper(8, 8);

        // --- 2. אתחול ה-UI ---
        std::string assetsDir = R"(C:\Users\USER\Desktop\chess game\UI\asset)";
        Renderer renderer(assetsDir);
        Controller controller(board, mapper, engine);

        // חיבור למשתנים הגלובליים
        g_controller = &controller;
        g_engine = &engine;
        g_renderer = &renderer;

        // --- 3. הרצה ---
        cv::namedWindow("Image");
        cv::setMouseCallback("Image", onMouseClick, nullptr);

        std::cout << "Game started! Click to move." << std::endl;

        // לולאת הרצה ראשית
        while (true) {
            // 1. קידום הזמן במנוע
            engine.wait(20); 

            // 2. ציור הלוח העדכני (שימוש ב-engine המקומי)
            GameSnapshot snap = engine.getSnapshot();
            renderer.render(snap, engine, controller);
            
            // 3. עיבוד אירועי עכבר ויציאה עם ESC
            if (cv::waitKey(20) == 27) break; 
        }
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}